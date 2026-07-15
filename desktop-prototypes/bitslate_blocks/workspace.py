import json
from PySide6.QtCore import Qt, QRectF, QPointF
from PySide6.QtGui import QPainter
from PySide6.QtWidgets import QGraphicsScene, QGraphicsView
from block_models import DEFINITIONS, make_model
from block_items import BlockItem
from palette import MIME
from run_controls import RunControls

class WorkspaceScene(QGraphicsScene):
    def __init__(self): super().__init__(QRectF(0,0,1200,800)); self.setBackgroundBrush(Qt.GlobalColor.white)
    def add_block(self,opcode,pos):
        definition=DEFINITIONS[opcode]; model=make_model(definition); item=BlockItem(definition,model,self); item.setPos(pos); self.addItem(item); return item
    def try_snap(self,item):
        if item.definition.shape=="reporter": return
        candidates=[x for x in self.items() if isinstance(x,BlockItem) and x is not item and x.definition.shape!="reporter" and item not in x.children_stack]
        if not candidates:return
        above=min(candidates,key=lambda x:(x.connector_bottom()-item.connector_top()).manhattanLength())
        if (above.connector_bottom()-item.connector_top()).manhattanLength()<38:
            target=above.scenePos()+QPointF(0,above.height-10); item.setPos(target); above.children_stack.append(item)
            above.model.next_block=item.model.id; item.model.parent_block=above.model.id
    def delete_selected(self):
        for item in self.selectedItems():
            if isinstance(item,BlockItem):
                for other in self.items():
                    if isinstance(other,BlockItem) and item in other.children_stack: other.children_stack.remove(item); other.model.next_block=None
                self.removeItem(item)

class WorkspaceView(QGraphicsView):
    def __init__(self):
        self.workspace=WorkspaceScene(); super().__init__(self.workspace); self.setAcceptDrops(True); self.setRenderHint(QPainter.RenderHint.Antialiasing,False)
        # Full redraws trade a little performance for correctness and eliminate
        # stale outline/shadow regions while movable items cross the viewport.
        self.setViewportUpdateMode(QGraphicsView.ViewportUpdateMode.FullViewportUpdate)
        self.setCacheMode(QGraphicsView.CacheModeFlag.CacheNone)
        self.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded);self.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.setDragMode(QGraphicsView.DragMode.NoDrag); self._panning=False; self._space=False; self._last=QPointF(); self.setFocusPolicy(Qt.FocusPolicy.StrongFocus)
        self.run_controls=RunControls(self.viewport());self.run_controls.show();self._position_run_controls()
    def _position_run_controls(self):
        pad=18;self.run_controls.move(self.viewport().width()-self.run_controls.width()-pad,self.viewport().height()-self.run_controls.height()-pad);self.run_controls.raise_()
    def resizeEvent(self,event):
        super().resizeEvent(event);self._position_run_controls()
    def dragEnterEvent(self,e): e.acceptProposedAction() if e.mimeData().hasFormat(MIME) else super().dragEnterEvent(e)
    def dragMoveEvent(self,e): e.acceptProposedAction() if e.mimeData().hasFormat(MIME) else super().dragMoveEvent(e)
    def dropEvent(self,e):
        if e.mimeData().hasFormat(MIME):
            data=json.loads(bytes(e.mimeData().data(MIME))); self.workspace.add_block(data["opcode"],self.mapToScene(e.position().toPoint())); e.acceptProposedAction()
    def keyPressEvent(self,e):
        if e.key() in (Qt.Key.Key_Delete,Qt.Key.Key_Backspace): self.workspace.delete_selected(); return
        if e.key()==Qt.Key.Key_Space: self._space=True; self.setCursor(Qt.CursorShape.OpenHandCursor); return
        super().keyPressEvent(e)
    def keyReleaseEvent(self,e):
        if e.key()==Qt.Key.Key_Space:self._space=False;self.unsetCursor();return
        super().keyReleaseEvent(e)
    def mousePressEvent(self,e):
        if e.button()==Qt.MouseButton.MiddleButton or (self._space and e.button()==Qt.MouseButton.LeftButton): self._panning=True;self._last=e.position();self.setCursor(Qt.CursorShape.ClosedHandCursor);e.accept();return
        super().mousePressEvent(e)
    def mouseMoveEvent(self,e):
        if self._panning:
            d=e.position()-self._last;self._last=e.position();self.horizontalScrollBar().setValue(self.horizontalScrollBar().value()-int(d.x()));self.verticalScrollBar().setValue(self.verticalScrollBar().value()-int(d.y()));return
        super().mouseMoveEvent(e)
    def mouseReleaseEvent(self,e):
        if self._panning:self._panning=False;self.unsetCursor();return
        super().mouseReleaseEvent(e)
    def wheelEvent(self,e):
        if e.modifiers() & Qt.KeyboardModifier.ControlModifier:self.scale_by(1.15 if e.angleDelta().y()>0 else 1/1.15);e.accept();return
        super().wheelEvent(e)
    def scale_by(self,factor):
        current=self.transform().m11(); factor=max(.25/current,min(3.0/current,factor)); self.scale(factor,factor)
    def reset_zoom(self): self.resetTransform()

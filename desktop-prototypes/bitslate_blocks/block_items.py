from PySide6.QtCore import Qt, QRectF, QPointF
from PySide6.QtGui import QColor, QPainter, QPen, QFontMetrics
from PySide6.QtWidgets import QGraphicsItem, QGraphicsProxyWidget, QLineEdit
from block_shapes import block_path, highlight_path
from styles import CATEGORIES, darken, pixel_font
from run_controls import paint_pixel_button

class BlockItem(QGraphicsItem):
    HEIGHTS = {"stack": 66, "hat": 76, "reporter": 60, "c": 166}
    MIN_WIDTHS = {"stack": 210, "hat": 270, "reporter": 180, "c": 260}
    PAD, GAP = 20, 7
    INPUT_RAISE = 7

    def __init__(self, definition, model, workspace=None, palette=False):
        super().__init__(); self.definition=definition; self.model=model; self.workspace=workspace; self.palette=palette
        self.height=self.HEIGHTS[definition.shape]; self.width=self.MIN_WIDTHS[definition.shape]
        self.children_stack=[]; self.inputs=[]; self._layout=[]
        flags=QGraphicsItem.GraphicsItemFlag.ItemIsSelectable
        if not palette: flags |= QGraphicsItem.GraphicsItemFlag.ItemIsMovable | QGraphicsItem.GraphicsItemFlag.ItemSendsGeometryChanges
        self.setFlags(flags); self.setCacheMode(QGraphicsItem.CacheMode.NoCache); self.setZValue(2)
        self._build_inputs(); self._relayout()

    def boundingRect(self):
        # Includes the full 4 px outline and attached 6 px lower/right shadow.
        return QRectF(0,0,self.width,self.height)
    def shape(self): return block_path(self.width,self.height,self.definition.shape)

    def _input_width(self, value, kind):
        fm=QFontMetrics(pixel_font(10)); minimum=70 if kind=="text" else 48
        return max(minimum,min(130,fm.horizontalAdvance(value or " ")+24))
    def _build_inputs(self):
        for part in self.definition.parts:
            if not isinstance(part,tuple): continue
            name,kind=part
            if kind=="play_icon":
                continue
            edit=QLineEdit(str(self.model.arguments.get(name,"")))
            edit.setFont(pixel_font(10)); edit.setAlignment(Qt.AlignmentFlag.AlignCenter); edit.setMaxLength(18)
            edit.setFixedSize(self._input_width(edit.text(),kind),32)
            edit.setStyleSheet("QLineEdit{background:#fffaf0;color:#30251e;border:3px solid #6f5432;border-radius:8px;padding:1px;} QLineEdit:focus{border-color:#fff1a6;}")
            proxy=QGraphicsProxyWidget(self); proxy.setWidget(edit); proxy.setZValue(4)
            edit.textChanged.connect(lambda value,n=name,k=kind,e=edit:self._input_changed(n,k,e,value))
            self.inputs.append((name,kind,proxy,edit))
    def _relayout(self):
        fm=QFontMetrics(pixel_font(11)); x=self.PAD; layout=[]
        for part in self.definition.parts:
            if isinstance(part,str):
                w=fm.horizontalAdvance(part); layout.append(("text",part,x,w)); x+=w+self.GAP
            else:
                if part[1]=="play_icon":
                    w=30;layout.append(("play_icon",None,x,w));x+=w+self.GAP
                else:
                    entry=next(v for v in self.inputs if v[0]==part[0]); w=entry[3].width(); layout.append(("input",entry,x,w)); x+=w+self.GAP
        new_width=max(self.MIN_WIDTHS[self.definition.shape],x+self.PAD)
        if new_width != self.width: self.prepareGeometryChange(); self.width=int(new_width)
        input_y=(18 if self.definition.shape=="hat" else 13)-self.INPUT_RAISE
        self.input_y=input_y
        for kind,data,x,w in layout:
            if kind=="input": data[2].setPos(int(x),input_y)
        self._layout=layout; self.update()
    def _input_changed(self,name,kind,edit,value):
        self.model.arguments[name]=value; new=self._input_width(value,kind)
        if edit.width()!=new: edit.setFixedWidth(new); self._relayout()
        else: self.update()

    def paint(self,painter,option,widget=None):
        painter.save(); painter.setRenderHint(QPainter.RenderHint.Antialiasing,False)
        color=CATEGORIES[self.definition.category][2]; path=block_path(self.width,self.height,self.definition.shape)
        painter.setBrush(color); painter.setPen(QPen(darken(color,185),4)); painter.drawPath(path)
        painter.save(); painter.setClipPath(path); painter.setPen(QPen(color.lighter(145),2)); painter.drawPath(highlight_path(self.width,self.definition.shape)); painter.restore()
        if self.isSelected(): painter.setPen(QPen(QColor("#ffe56a"),3,Qt.PenStyle.DashLine)); painter.drawPath(path)
        painter.setFont(pixel_font(11)); painter.setPen(QColor("white")); baseline=42 if self.definition.shape=="hat" else 36
        for kind,data,x,w in self._layout:
            if kind=="text": painter.drawText(QPointF(int(x),baseline),data)
            elif kind=="play_icon":
                paint_pixel_button(painter,QRectF(int(x),17,28,31),"play")
        painter.restore()

    def itemChange(self,change,value):
        if change==QGraphicsItem.GraphicsItemChange.ItemPositionChange and not self.palette:
            delta=value-self.pos()
            for child in tuple(self.children_stack): child.setPos(child.pos()+delta)
        if change==QGraphicsItem.GraphicsItemChange.ItemPositionHasChanged and not self.palette:
            self.model.x=value.x();self.model.y=value.y();self.update()
        return super().itemChange(change,value)
    def mouseReleaseEvent(self,event):
        super().mouseReleaseEvent(event)
        if self.workspace:self.workspace.try_snap(self)
    def connector_bottom(self):return self.scenePos()+QPointF(51,self.height-8)
    def connector_top(self):return self.scenePos()+QPointF(51,3)

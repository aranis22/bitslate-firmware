"""Clickable pixel-art toolbars for the future Output Preview surface."""
from pathlib import Path
from PySide6.QtCore import Qt, QSize, QPoint, QRect, Signal
from PySide6.QtGui import QColor, QPainter, QPen, QPixmap, QPolygon
from PySide6.QtWidgets import QWidget, QPushButton, QHBoxLayout, QStyleOption, QStyle

ASSET_DIR = Path(__file__).resolve().parents[2] / "src" / "assets" / "UI" / "bitblocks-ui"

class PixelIconButton(QPushButton):
    def __init__(self,filename,bottom=False,parent=None):
        super().__init__(parent);self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground,True);self.filename=filename;self.bottom=bottom;self.setFixedSize(46,48 if bottom else 43)
        source=QPixmap(str(ASSET_DIR/filename));self.icon_pixmap=source.scaled(30,30,Qt.AspectRatioMode.KeepAspectRatio,Qt.TransformationMode.FastTransformation)
        self.setCursor(Qt.CursorShape.PointingHandCursor);self.setToolTip(filename.removesuffix(".png").replace("_"," ").title())
    def paintEvent(self,event):
        p=QPainter(self);p.setRenderHint(QPainter.RenderHint.Antialiasing,False)
        if self.bottom:
            pts=[(8,1),(38,1),(38,4),(42,4),(42,9),(45,9),(45,37),(42,37),(42,42),(37,42),(37,46),(9,46),(9,44),(4,44),(4,40),(1,40),(1,9),(4,9),(4,4),(8,4)]
            poly=QPolygon([QPoint(x,y) for x,y in pts]);p.setPen(QPen(QColor("#245f42"),2));p.setBrush(QColor("#4dbb76"));p.drawPolygon(poly)
            p.setPen(Qt.PenStyle.NoPen);p.setBrush(QColor("#78d795"));p.drawRect(8,6,30,3);p.setBrush(QColor("#36965e"));p.drawRect(7,37,32,6)
        if self.isDown():p.fillRect(self.rect().adjusted(3,3,-3,-3),QColor(255,255,255,65))
        x=(self.width()-self.icon_pixmap.width())//2;y=(self.height()-self.icon_pixmap.height())//2-1;p.drawPixmap(x,y,self.icon_pixmap);p.end()

class PreviewToolbar(QWidget):
    expandedChanged=Signal(bool)
    def __init__(self,bottom_asset,parent=None):
        super().__init__(parent);self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground,True);self.expanded=False;self.setFixedSize(52,51);self.buttons=[]
        for i,name in enumerate(("sparkle.png","paint.png","search.png")):
            button=PixelIconButton(name,parent=self);button.move(3,10+i*43);button.hide();self.buttons.append(button)
        self.trigger=PixelIconButton(bottom_asset,True,self);self.trigger.move(3,1);self.trigger.clicked.connect(self.toggle_expanded);self.buttons.append(self.trigger)
    def toggle_expanded(self):
        self.expanded=not self.expanded
        for button in self.buttons[:-1]:button.setVisible(self.expanded)
        self.setFixedHeight(190 if self.expanded else 51);self.trigger.move(3,139 if self.expanded else 1)
        self.update();self.expandedChanged.emit(self.expanded)
    def paintEvent(self,event):
        p=QPainter(self);p.setRenderHint(QPainter.RenderHint.Antialiasing,False)
        if not self.expanded:
            p.end();return
        outer=QPolygon([QPoint(7,0),QPoint(45,0),QPoint(45,3),QPoint(49,3),QPoint(49,136),QPoint(45,136),QPoint(45,141),QPoint(7,141),QPoint(7,138),QPoint(3,138),QPoint(3,4),QPoint(7,4)])
        inner=QPolygon([QPoint(8,5),QPoint(44,5),QPoint(44,8),QPoint(47,8),QPoint(47,132),QPoint(43,132),QPoint(43,136),QPoint(8,136),QPoint(8,133),QPoint(5,133),QPoint(5,8),QPoint(8,8)])
        p.setPen(Qt.PenStyle.NoPen);p.setBrush(QColor("#153681"));p.drawPolygon(outer);p.setBrush(QColor("#2d62cf"));p.drawPolygon(inner)
        p.setBrush(QColor("#4d7ee0"));p.drawRect(9,7,33,3);p.setBrush(QColor("#204fae"));p.drawRect(7,128,38,7);p.end()

class PreviewToolbarPair(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent);self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground,True);self.setFixedSize(114,51);layout=QHBoxLayout(self);layout.setContentsMargins(0,0,0,0);layout.setSpacing(10)
        self.left_toolbar=PreviewToolbar("capybara_plus.png");self.right_toolbar=PreviewToolbar("image.png")
        layout.addWidget(self.left_toolbar,0,Qt.AlignmentFlag.AlignBottom);layout.addWidget(self.right_toolbar,0,Qt.AlignmentFlag.AlignBottom)
        self.left_toolbar.expandedChanged.connect(self._sync_height);self.right_toolbar.expandedChanged.connect(self._sync_height)
    def _sync_height(self,expanded):
        self.setFixedHeight(190 if self.left_toolbar.expanded or self.right_toolbar.expanded else 51)
        canvas=self.parentWidget()
        if canvas and hasattr(canvas,"_position_toolbars"):canvas._position_toolbars()

class PreviewCanvas(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent);self.setAttribute(Qt.WidgetAttribute.WA_StyledBackground,True);self.current_sprite_path=None;self.sprite_pixmap=QPixmap();self.current_backdrop_path=None;self.backdrop_pixmap=QPixmap();self.toolbars=PreviewToolbarPair(self);self.toolbars.show()
    def set_sprite(self,path):
        self.current_sprite_path=str(path) if path else None;self.sprite_pixmap=QPixmap(str(path)) if path else QPixmap();self.update();self.toolbars.raise_()
    def set_backdrop(self,path):
        self.current_backdrop_path=str(path) if path else None;self.backdrop_pixmap=QPixmap(str(path)) if path else QPixmap();self.update();self.toolbars.raise_()
    def paintEvent(self,event):
        p=QPainter(self);option=QStyleOption();option.initFrom(self);self.style().drawPrimitive(QStyle.PrimitiveElement.PE_Widget,option,p,self)
        if not self.backdrop_pixmap.isNull():
            inner=QRect(4,4,max(1,self.width()-8),max(1,self.height()-8));backdrop=self.backdrop_pixmap.scaled(inner.size(),Qt.AspectRatioMode.KeepAspectRatioByExpanding,Qt.TransformationMode.FastTransformation)
            p.save();p.setClipRect(inner);p.drawPixmap(inner.center().x()-backdrop.width()//2,inner.center().y()-backdrop.height()//2,backdrop);p.restore()
        if not self.sprite_pixmap.isNull():
            max_size=QSize(max(1,int(self.width()*.65)),max(1,int(self.height()*.70)))
            sprite=self.sprite_pixmap.scaled(max_size,Qt.AspectRatioMode.KeepAspectRatio,Qt.TransformationMode.FastTransformation)
            p.drawPixmap((self.width()-sprite.width())//2,(self.height()-sprite.height())//2,sprite)
        p.end()
    def resizeEvent(self,event):
        super().resizeEvent(event);self._position_toolbars()
    def _position_toolbars(self):
        side_pad=12
        # Reserve one stable bottom edge for both states. On compact 3:2
        # canvases the padding reduces just enough for the 190 px panel to fit.
        bottom_pad=min(12,max(0,(self.height()-190)//2))
        self.toolbars.move(max(0,self.width()-self.toolbars.width()-side_pad),max(0,self.height()-self.toolbars.height()-bottom_pad));self.toolbars.raise_()

class AspectRatioPreview(QWidget):
    """Centers a responsive BitSlate 480x320 (3:2) preview without stretching."""
    DESIGN_WIDTH,DESIGN_HEIGHT=480,320
    def __init__(self,parent=None):
        super().__init__(parent);self.canvas=PreviewCanvas(self)
    def resizeEvent(self,event):
        super().resizeEvent(event)
        available_w=min(self.width(),self.DESIGN_WIDTH);available_h=min(self.height(),self.DESIGN_HEIGHT)
        height=min(available_h,(available_w*2)//3);height-=height%2
        width=height*3//2
        if width>available_w:
            width=available_w-(available_w%3);height=width*2//3
        # Keep any unavoidable vertical letterbox above the display so the
        # BitSlate preview sits as low as possible beneath the larger palette.
        self.canvas.setGeometry((self.width()-width)//2,self.height()-height,width,height)

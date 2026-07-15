import json
from PySide6.QtCore import Qt, QMimeData, QPointF
from PySide6.QtGui import QDrag, QPixmap, QPainter, QColor, QPen, QFontMetrics
from PySide6.QtWidgets import QWidget, QVBoxLayout, QPushButton, QScrollArea, QLabel, QFrame, QSplitter
from block_items import BlockItem
from block_models import BLOCKS, make_model
from styles import CATEGORIES, darken, pixel_font
from preview_tools import AspectRatioPreview

MIME="application/x-bitslate-block"

class PaletteBlock(QWidget):
    """Programmatic preview using the same geometry as its workspace clone."""
    def __init__(self,definition):
        super().__init__(); self.definition=definition
        self.item=BlockItem(definition,make_model(definition),palette=True)
        for _,_,proxy,_ in self.item.inputs: proxy.hide()
        self.setFixedHeight(self.item.height+4); self.setMinimumWidth(self.item.width+16)
        self.setCursor(Qt.CursorShape.OpenHandCursor)
    def _paint_block(self,p):
        """Paint only block-owned pixels; safe for a transparent drag pixmap."""
        p.save();p.setCompositionMode(QPainter.CompositionMode.CompositionMode_SourceOver)
        p.setRenderHint(QPainter.RenderHint.Antialiasing,False)
        ox=max(8,int((self.width()-self.item.width)/2)); p.translate(ox,2); self.item.paint(p,None)
        # Proxy widgets cannot paint in a plain QWidget preview, so render their
        # exact fields here from the shared layout and model defaults.
        p.setFont(pixel_font(10)); fm=QFontMetrics(p.font())
        y=self.item.input_y+2
        for kind,data,x,w in self.item._layout:
            if kind!="input":continue
            name=data[0]; value=str(self.item.model.arguments[name]); rect=(int(x),y,int(w),32)
            p.setPen(Qt.PenStyle.NoPen);p.setBrush(QColor(43,31,22,90));p.drawRoundedRect(rect[0]+3,rect[1]+4,rect[2],rect[3],8,8)
            p.setBrush(QColor("#fffaf0"));p.setPen(QPen(QColor("#6f5432"),3));p.drawRoundedRect(*rect,8,8)
            p.setPen(QColor("#30251e"));p.drawText(rect[0],rect[1],rect[2],rect[3],Qt.AlignmentFlag.AlignCenter,value)
        p.restore()
    def paintEvent(self,event):
        p=QPainter(self);self._paint_block(p);p.end()
    def drag_pixmap(self):
        # Never call QWidget.render() here: it also captures the charcoal palette
        # background. A freshly cleared QPixmap retains alpha outside the shape.
        pix=QPixmap(self.size());pix.fill(Qt.GlobalColor.transparent)
        p=QPainter(pix);p.setCompositionMode(QPainter.CompositionMode.CompositionMode_SourceOver);self._paint_block(p);p.end()
        return pix
    def mousePressEvent(self,event): self._press=event.position()
    def mouseMoveEvent(self,event):
        if not(event.buttons()&Qt.MouseButton.LeftButton) or (event.position()-getattr(self,"_press",event.position())).manhattanLength()<6:return
        mime=QMimeData();mime.setData(MIME,json.dumps({"opcode":self.definition.opcode}).encode())
        drag=QDrag(self);drag.setMimeData(mime);drag.setPixmap(self.drag_pixmap());drag.setHotSpot(event.position().toPoint());drag.exec(Qt.DropAction.CopyAction)

class CategoryButton(QPushButton):
    def __init__(self,key):
        label,icon,color=CATEGORIES[key];super().__init__(f"{icon}   {label.upper()}");self.key=key
        self.setFont(pixel_font(10));self.setFixedHeight(46);self.setCheckable(True)
        self.setStyleSheet(f"QPushButton{{text-align:left;padding-left:14px;color:white;background:{color.name()};border:3px solid {darken(color,190).name()};border-bottom-width:5px;}} QPushButton:hover,QPushButton:checked{{background:{color.lighter(118).name()};border-color:#fff2a8;}}")

class CategoryHeader(QPushButton):
    def __init__(self):
        super().__init__();self.arrow=QLabel("v",self);self.arrow.setFont(pixel_font(11));self.arrow.setAlignment(Qt.AlignmentFlag.AlignCenter);self.arrow.setStyleSheet("color:white;background:transparent;");self.arrow.setAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents,True)
    def resizeEvent(self,event):
        super().resizeEvent(event);self.arrow.setGeometry(self.width()-42,0,32,self.height()-4)

class OutputPreview(QWidget):
    def __init__(self):
        super().__init__();layout=QVBoxLayout(self);layout.setContentsMargins(0,1,0,0);layout.setSpacing(1)
        title=QLabel("OUTPUT PREVIEW");title.setFixedHeight(18);title.setFont(pixel_font(9));title.setStyleSheet("color:#30384b;padding-left:4px;");layout.addWidget(title)
        self.preview_host=AspectRatioPreview();self.canvas=self.preview_host.canvas;self.canvas.setObjectName("outputCanvas");self.canvas.setStyleSheet("QWidget#outputCanvas{background:#fafafa;border:4px solid #29231f;}");layout.addWidget(self.preview_host,1)

class Palette(QWidget):
    def __init__(self):
        super().__init__();root=QVBoxLayout(self);root.setContentsMargins(7,7,7,7);root.setSpacing(5);self.buttons=[];self.current_category="movement";self._split_initialized=False
        self.header=CategoryHeader();self.header.setFont(pixel_font(11));self.header.setFixedHeight(56);self.header.clicked.connect(self.toggle_categories);root.addWidget(self.header)
        # Popup keeps the selector compact without resizing either sidebar pane.
        self.dropdown=QFrame(None,Qt.WindowType.Popup);self.dropdown.setStyleSheet("QFrame{background:#242424;border:3px solid #302820;}")
        choices=QVBoxLayout(self.dropdown);choices.setContentsMargins(5,5,5,5);choices.setSpacing(4)
        for key in CATEGORIES:
            b=CategoryButton(key);b.clicked.connect(lambda checked,k=key:self.show_category(k));choices.addWidget(b);self.buttons.append(b)
        self.scroll=QScrollArea();self.scroll.setWidgetResizable(True);self.scroll.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAsNeeded)
        self.scroll.setStyleSheet("QScrollArea{background:#242424;border:3px solid #382d27;} QScrollBar:vertical{background:#303030;width:13px;} QScrollBar::handle:vertical{background:#777;min-height:28px;}")
        self.output_preview=OutputPreview();self.splitter=QSplitter(Qt.Orientation.Vertical);self.splitter.setChildrenCollapsible(False)
        self.splitter.setStyleSheet("QSplitter::handle{background:#81766f;height:5px;}");self.splitter.addWidget(self.scroll);self.splitter.addWidget(self.output_preview)
        self.splitter.setStretchFactor(0,1);self.splitter.setStretchFactor(1,0);root.addWidget(self.splitter,1);self.show_category("movement")
    def _set_header_style(self,key):
        label,icon,color=CATEGORIES[key];self.header.setText(f"{icon}   {label.upper()}")
        self.header.setStyleSheet(f"QPushButton{{text-align:left;padding-left:16px;color:white;background:{color.name()};border:4px solid {darken(color,190).name()};border-bottom-width:7px;}} QPushButton:hover{{background:{color.lighter(112).name()};}}")
    def toggle_categories(self):
        if self.dropdown.isVisible():self.dropdown.hide();return
        self.dropdown.setFixedWidth(self.header.width());self.dropdown.adjustSize();self.dropdown.move(self.header.mapToGlobal(QPointF(0,self.header.height()).toPoint()));self.dropdown.show();self.dropdown.raise_()
    def show_category(self,key):
        self.current_category=key;self.dropdown.hide();self._set_header_style(key)
        for b in self.buttons:b.setChecked(b.key==key)
        content=QWidget();content.setStyleSheet("background:#242424;");layout=QVBoxLayout(content);layout.setContentsMargins(8,8,8,12);layout.setSpacing(7);layout.setAlignment(Qt.AlignmentFlag.AlignTop)
        if not BLOCKS[key]:
            label=QLabel("No camera blocks yet.");label.setAlignment(Qt.AlignmentFlag.AlignCenter);label.setFont(pixel_font(11));label.setStyleSheet("color:#e8e3ed;padding:30px;");layout.addWidget(label)
        for definition in BLOCKS[key]:layout.addWidget(PaletteBlock(definition),0,Qt.AlignmentFlag.AlignHCenter)
        self.scroll.setWidget(content)
    def showEvent(self,event):
        super().showEvent(event)
        if not self._split_initialized:
            available=max(200,self.splitter.height())
            # Reserve only the display's responsive 3:2 height plus its compact
            # title; the scrollable block palette receives all remaining space.
            preview_width=min(480,max(180,self.splitter.width()))
            preview_height=min(320,(preview_width*2)//3)+20
            preview_height=min(preview_height,int(available*.58))
            self.splitter.setSizes([available-preview_height,preview_height]);self._split_initialized=True

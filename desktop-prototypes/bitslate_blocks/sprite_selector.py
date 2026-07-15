from math import ceil
from pathlib import Path
from PySide6.QtCore import Qt, Signal, QRect, QPoint
from PySide6.QtGui import QColor, QPainter, QPen, QPixmap, QLinearGradient, QPolygon
from PySide6.QtWidgets import QWidget, QPushButton
from styles import pixel_font

SPRITE_DIR=Path(__file__).resolve().parents[2]/"src"/"assets"/"UI"/"sprites"
BACKDROP_DIR=Path(__file__).resolve().parents[2]/"src"/"assets"/"UI"/"backdrops"

class SpriteCard(QWidget):
    clicked=Signal(str);doubleClicked=Signal(str)
    def __init__(self,parent=None):
        super().__init__(parent);self.path=None;self.pixmap=QPixmap();self.selected=False;self.setCursor(Qt.CursorShape.PointingHandCursor)
    def set_sprite(self,path):
        self.path=path;self.pixmap=QPixmap(str(path)) if path else QPixmap();self.show() if path else self.hide();self.update()
    def paintEvent(self,event):
        if not self.path:return
        p=QPainter(self);p.setRenderHint(QPainter.RenderHint.Antialiasing,False);w=self.width();frame_h=self.height()-38
        border=QColor("#ffe86d") if self.selected else QColor("#d9d6e2")
        p.setPen(QPen(QColor("#1d122d"),5));p.setBrush(QColor("#f6f4f8"));p.drawRect(3,3,w-7,frame_h-4)
        p.setPen(QPen(border,4));p.drawRect(9,9,w-19,frame_h-16)
        target=QRect(18,18,w-36,frame_h-34);scaled=self.pixmap.scaled(target.size(),Qt.AspectRatioMode.KeepAspectRatio,Qt.TransformationMode.FastTransformation)
        p.drawPixmap(target.x()+(target.width()-scaled.width())//2,target.y()+(target.height()-scaled.height())//2,scaled)
        p.setFont(pixel_font(12));p.setPen(QColor("white"));p.drawText(QRect(0,frame_h+3,w,32),Qt.AlignmentFlag.AlignCenter,self.path.stem);p.end()
    def mousePressEvent(self,event):
        if event.button()==Qt.MouseButton.LeftButton and self.path:self.clicked.emit(str(self.path))
    def mouseDoubleClickEvent(self,event):
        if event.button()==Qt.MouseButton.LeftButton and self.path:self.doubleClicked.emit(str(self.path))

class PixelPageScrollbar(QWidget):
    pageRequested=Signal(int)
    def __init__(self,parent=None):
        super().__init__(parent);self.page=0;self.pages=1;self.setFixedWidth(42)
    def set_state(self,page,pages):self.page=page;self.pages=max(1,pages);self.update()
    def paintEvent(self,event):
        p=QPainter(self);p.setRenderHint(QPainter.RenderHint.Antialiasing,False);w=self.width();h=self.height()
        p.setPen(QPen(QColor("#160d25"),4));p.setBrush(QColor("#f5f3f6"));p.drawRect(5,2,w-10,h-4)
        p.setBrush(QColor("#d6d2dc"));p.drawRect(9,7,w-18,28);p.drawRect(9,h-35,w-18,28)
        p.setPen(Qt.PenStyle.NoPen);p.setBrush(QColor("#1c1725"))
        p.drawPolygon(QPolygon([QPoint(w//2,12),QPoint(13,29),QPoint(w-13,29)]));p.drawPolygon(QPolygon([QPoint(13,h-29),QPoint(w-13,h-29),QPoint(w//2,h-12)]))
        track_top=42;track_h=max(20,h-84);thumb_h=max(24,track_h//self.pages);travel=max(0,track_h-thumb_h);y=track_top+(travel*self.page//max(1,self.pages-1))
        p.setBrush(QColor("#211a2a"));p.drawRect(12,y,w-24,thumb_h);p.end()
    def mousePressEvent(self,event):
        if event.position().y()<40:self.pageRequested.emit(self.page-1)
        elif event.position().y()>self.height()-40:self.pageRequested.emit(self.page+1)
        else:
            ratio=(event.position().y()-42)/max(1,self.height()-84);self.pageRequested.emit(round(ratio*(self.pages-1)))

class SpriteSelectorPage(QWidget):
    backRequested=Signal();confirmed=Signal(str)
    def __init__(self,parent=None,asset_dir=SPRITE_DIR,title="SPRITE SELECTOR",colors=("#2b1746","#4b2370","#6e4097")):
        super().__init__(parent);self.setFocusPolicy(Qt.FocusPolicy.StrongFocus);self.selector_title=title;self.background_colors=colors
        self.sprite_paths=sorted(Path(asset_dir).glob("*.png"),key=lambda p:p.stem.lower());self.page=0;self.selected_path=None
        self.cards=[SpriteCard(self) for _ in range(4)]
        for card in self.cards:card.clicked.connect(self.select_path);card.doubleClicked.connect(self.confirm_selected_path)
        self.scrollbar=PixelPageScrollbar(self);self.scrollbar.pageRequested.connect(self.set_page)
        self.back=QPushButton("< BACK",self);self.confirm=QPushButton("CONFIRM >",self)
        for button in (self.back,self.confirm):button.setFont(pixel_font(10));button.setFixedHeight(38);button.setStyleSheet("QPushButton{color:white;background:#5d358b;border:4px solid #21132f;padding:4px 14px;} QPushButton:pressed{background:#8050ad;}")
        self.back.clicked.connect(self.backRequested);self.confirm.clicked.connect(self.confirm_selection);self.refresh_page()
    @property
    def page_count(self):return max(1,ceil(len(self.sprite_paths)/4))
    def showEvent(self,event):super().showEvent(event);self.setFocus(Qt.FocusReason.OtherFocusReason)
    def paintEvent(self,event):
        p=QPainter(self);p.setRenderHint(QPainter.RenderHint.Antialiasing,False);g=QLinearGradient(0,0,0,self.height());g.setColorAt(0,QColor(self.background_colors[0]));g.setColorAt(1,QColor(self.background_colors[1]));p.fillRect(self.rect(),g)
        accent=QColor(self.background_colors[2]);accent.setAlpha(95);p.setPen(Qt.PenStyle.NoPen);p.setBrush(accent)
        for y in range(20,self.height(),72):
            offset=0 if (y//72)%2 else 28
            for x in range(-offset,self.width(),84):p.drawRect(x,y,68,12)
        p.setFont(pixel_font(16));p.setPen(QColor("white"));p.drawText(QRect(24,18,self.width()-48,42),Qt.AlignmentFlag.AlignCenter,self.selector_title);p.end()
    def resizeEvent(self,event):
        super().resizeEvent(event);w=self.width();h=self.height();scroll_w=42;side=50;gap=18
        available=w-side*2-scroll_w-30;card_w=max(140,min(270,(available-gap*3)//4));card_h=min(330,max(190,int(card_w*1.15)))
        total=card_w*4+gap*3;start=max(24,(w-scroll_w-20-total)//2);y=max(78,(h-card_h)//2)
        for i,card in enumerate(self.cards):card.setGeometry(start+i*(card_w+gap),y,card_w,card_h)
        self.scrollbar.setGeometry(w-scroll_w-18,82,scroll_w,max(180,h-150));self.back.setGeometry(24,h-58,110,38);self.confirm.setGeometry(w-164,h-58,140,38)
    def refresh_page(self):
        start=self.page*4
        for i,card in enumerate(self.cards):
            path=self.sprite_paths[start+i] if start+i<len(self.sprite_paths) else None;card.set_sprite(path);card.selected=bool(path and str(path)==self.selected_path);card.update()
        self.scrollbar.set_state(self.page,self.page_count)
    def set_page(self,page):
        page=max(0,min(self.page_count-1,int(page)))
        if page!=self.page:self.page=page;self.refresh_page()
    def select_path(self,path):
        self.selected_path=path;self.refresh_page()
    def set_current_path(self,path):
        path=str(path) if path else None;self.selected_path=path
        matches=[i for i,p in enumerate(self.sprite_paths) if str(p)==path]
        if matches:self.page=matches[0]//4
        self.refresh_page()
    def confirm_selection(self,path=None):
        if path:self.select_path(path)
        if self.selected_path:self.confirmed.emit(self.selected_path)
    def confirm_selected_path(self,path):
        if path==self.selected_path:self.confirmed.emit(path)
    def wheelEvent(self,event):
        self.set_page(self.page+(1 if event.angleDelta().y()<0 else -1));event.accept()
    def keyPressEvent(self,event):
        if event.key() in (Qt.Key.Key_Right,Qt.Key.Key_Down):self.set_page(self.page+1);return
        if event.key() in (Qt.Key.Key_Left,Qt.Key.Key_Up):self.set_page(self.page-1);return
        if event.key() in (Qt.Key.Key_Return,Qt.Key.Key_Enter):self.confirm_selection();return
        if event.key()==Qt.Key.Key_Escape:self.backRequested.emit();return
        super().keyPressEvent(event)

class BackdropSelectorPage(SpriteSelectorPage):
    def __init__(self,parent=None):
        super().__init__(parent,BACKDROP_DIR,"BACKDROP SELECTOR",("#35383f","#62666e","#8b9099"))

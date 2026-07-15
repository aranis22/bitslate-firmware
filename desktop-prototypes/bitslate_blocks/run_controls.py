"""Shared programmatic pixel-art play/stop button rendering."""
from PySide6.QtCore import Qt, QPoint
from PySide6.QtGui import QColor, QPainter, QPen, QPolygon
from PySide6.QtWidgets import QWidget

def paint_pixel_button(painter, rect, kind):
    painter.save(); painter.setRenderHint(QPainter.RenderHint.Antialiasing, False)
    x,y,w,h=map(int,(rect.x(),rect.y(),rect.width(),rect.height()))
    outline=QColor("#171820"); body=QColor("#4b843d") if kind=="play" else QColor("#df244b")
    lower=QColor("#32692f") if kind=="play" else QColor("#b7173b")
    highlight=QColor("#68a555") if kind=="play" else QColor("#f04465")
    poly=lambda points:QPolygon([QPoint(px,py) for px,py in points])
    if h < 40:
        # Compact inline variant: same palette and pixel layers without the
        # large control's deep bevel consuming the tiny Events-block icon.
        outer=poly([(x+4,y),(x+w-4,y),(x+w-4,y+2),(x+w-1,y+2),(x+w-1,y+5),(x+w,y+5),
                    (x+w,y+h-5),(x+w-2,y+h-5),(x+w-2,y+h-2),(x+w-5,y+h-2),
                    (x+w-5,y+h),(x+4,y+h),(x+4,y+h-2),(x+1,y+h-2),(x+1,y+h-5),(x,y+h-5),
                    (x,y+5),(x+2,y+5),(x+2,y+2),(x+4,y+2)])
        painter.setPen(Qt.PenStyle.NoPen);painter.setBrush(outline);painter.drawPolygon(outer)
        painter.setBrush(body);painter.drawRect(x+4,y+4,w-8,h-9)
        painter.setBrush(highlight);painter.drawRect(x+6,y+5,w-12,2)
        painter.setBrush(lower);painter.drawRect(x+4,y+h-10,w-8,5)
        painter.setBrush(QColor("#f4f4f2"));cx=x+w//2-3;cy=y+8
        painter.drawPolygon(poly([(cx,cy),(cx+4,cy),(cx+4,cy+3),(cx+7,cy+3),(cx+7,cy+7),
                                  (cx+10,cy+7),(cx+10,cy+11),(cx+7,cy+11),(cx+7,cy+15),
                                  (cx+4,cy+15),(cx+4,cy+18),(cx,cy+18)]))
        painter.restore();return
    outer=poly([(x+10,y),(x+w-10,y),(x+w-10,y+3),(x+w-4,y+3),(x+w-4,y+9),(x+w,y+9),
                    (x+w,y+h-12),(x+w-4,y+h-12),(x+w-4,y+h-5),(x+w-10,y+h-5),(x+w-10,y+h),
                    (x+10,y+h),(x+10,y+h-3),(x+4,y+h-3),(x+4,y+h-9),(x,y+h-9),(x,y+10),
                    (x+4,y+10),(x+4,y+4),(x+10,y+4)])
    painter.setPen(Qt.PenStyle.NoPen);painter.setBrush(outline);painter.drawPolygon(outer)
    inner=poly([(x+11,y+5),(x+w-11,y+5),(x+w-11,y+8),(x+w-7,y+8),(x+w-7,y+13),(x+w-4,y+13),
                    (x+w-4,y+h-15),(x+w-8,y+h-15),(x+w-8,y+h-9),(x+w-12,y+h-9),
                    (x+12,y+h-9),(x+12,y+h-12),(x+8,y+h-12),(x+8,y+h-17),(x+5,y+h-17),
                    (x+5,y+13),(x+8,y+13),(x+8,y+8),(x+11,y+8)])
    painter.setBrush(body);painter.drawPolygon(inner)
    painter.setBrush(lower);painter.drawRect(x+8,y+h-20,w-16,11)
    painter.setBrush(highlight);painter.drawRect(x+11,y+8,w-22,3)
    painter.setBrush(QColor("#f4f4f2"))
    if kind=="play":
        cx=x+w//2-5;cy=y+h//2-13
        icon=poly([(cx,cy),(cx+6,cy),(cx+6,cy+5),(cx+11,cy+5),(cx+11,cy+10),
                       (cx+16,cy+10),(cx+16,cy+16),(cx+11,cy+16),(cx+11,cy+21),
                       (cx+6,cy+21),(cx+6,cy+27),(cx,cy+27)])
        painter.drawPolygon(icon)
    else:
        side=max(8,min(w,h)//2-3);painter.drawRect(x+(w-side)//2,y+(h-side)//2-3,side,side)
    painter.restore()

class RunControls(QWidget):
    def __init__(self,parent=None):
        super().__init__(parent);self.setFixedSize(148,72)
        self.setAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents,True)
        self.setAttribute(Qt.WidgetAttribute.WA_TranslucentBackground,True)
    def paintEvent(self,event):
        p=QPainter(self);p.setCompositionMode(QPainter.CompositionMode.CompositionMode_SourceOver)
        paint_pixel_button(p,self.rect().adjusted(2,2,-78,-2),"play")
        paint_pixel_button(p,self.rect().adjusted(78,2,-2,-2),"stop");p.end()

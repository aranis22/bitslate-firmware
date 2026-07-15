"""Fresh, closed pixel-art paths shared by workspace and palette blocks."""
from PySide6.QtGui import QPainterPath

OUTLINE = 4
SHADOW = 6
BODY_RIGHT = 7
# Leaves room for the 7 px connector depth and 6 px attached shadow.
BODY_BOTTOM = 14
NOTCH_X = 34
NOTCH_W = 34
NOTCH_D = 7

def _top_notch(p, y=3):
    p.lineTo(NOTCH_X, y)
    p.lineTo(NOTCH_X + 4, y); p.lineTo(NOTCH_X + 4, y + 3)
    p.lineTo(NOTCH_X + 7, y + 3); p.lineTo(NOTCH_X + 7, y + NOTCH_D)
    p.lineTo(NOTCH_X + NOTCH_W - 7, y + NOTCH_D)
    p.lineTo(NOTCH_X + NOTCH_W - 7, y + 3)
    p.lineTo(NOTCH_X + NOTCH_W - 4, y + 3); p.lineTo(NOTCH_X + NOTCH_W - 4, y)
    p.lineTo(NOTCH_X + NOTCH_W, y)

def _bottom_connector(p, y):
    p.lineTo(NOTCH_X + NOTCH_W, y)
    p.lineTo(NOTCH_X + NOTCH_W - 4, y); p.lineTo(NOTCH_X + NOTCH_W - 4, y + 3)
    p.lineTo(NOTCH_X + NOTCH_W - 7, y + 3); p.lineTo(NOTCH_X + NOTCH_W - 7, y + NOTCH_D)
    p.lineTo(NOTCH_X + 7, y + NOTCH_D)
    p.lineTo(NOTCH_X + 7, y + 3); p.lineTo(NOTCH_X + 4, y + 3); p.lineTo(NOTCH_X + 4, y)
    p.lineTo(NOTCH_X, y)

def _top_right_corner(p, r):
    p.lineTo(r - 5, 3); p.lineTo(r - 5, 5); p.lineTo(r - 2, 5); p.lineTo(r - 2, 8); p.lineTo(r, 8)

def _bottom_left_corner(p, b):
    p.lineTo(8, b); p.lineTo(8, b - 2); p.lineTo(5, b - 2); p.lineTo(5, b - 5); p.lineTo(3, b - 5)

def block_path(width, height, shape):
    """Return one new closed silhouette. All coordinates fit width/height."""
    r, b = width - BODY_RIGHT, height - BODY_BOTTOM
    p = QPainterPath()
    if shape == "reporter":
        p.moveTo(13, 3); p.lineTo(r - 10, 3)
        p.lineTo(r - 10, 5); p.lineTo(r - 6, 5); p.lineTo(r - 6, 8)
        p.lineTo(r - 3, 8); p.lineTo(r - 3, 13); p.lineTo(r, 13)
        p.lineTo(r, b - 13); p.lineTo(r - 3, b - 13); p.lineTo(r - 3, b - 8)
        p.lineTo(r - 6, b - 8); p.lineTo(r - 6, b - 5); p.lineTo(r - 10, b - 5); p.lineTo(r - 10, b)
        p.lineTo(13, b); p.lineTo(13, b - 2); p.lineTo(9, b - 2); p.lineTo(9, b - 5)
        p.lineTo(6, b - 5); p.lineTo(6, b - 9); p.lineTo(3, b - 9)
        p.lineTo(3, 12); p.lineTo(6, 12); p.lineTo(6, 8); p.lineTo(9, 8)
        p.lineTo(9, 5); p.lineTo(13, 5); p.closeSubpath()
        return p
    if shape == "hat":
        # Assertive event hat: tall left shoulder, raised plateau and a chunky
        # diagonal transition into the main top edge (no soft curved bump).
        p.moveTo(3, 20); p.lineTo(42, 20); p.lineTo(42, 3)
        p.lineTo(66, 3); p.lineTo(66, 6); p.lineTo(71, 6); p.lineTo(71, 10)
        p.lineTo(76, 10); p.lineTo(76, 14); p.lineTo(81, 14); p.lineTo(81, 18)
        p.lineTo(86, 18); p.lineTo(86, 22); p.lineTo(91, 22); p.lineTo(91, 24); p.lineTo(r - 5, 24)
        p.lineTo(r - 5, 26); p.lineTo(r - 2, 26); p.lineTo(r - 2, 29); p.lineTo(r, 29)
    else:
        p.moveTo(8, 3); _top_notch(p); _top_right_corner(p, r)
    if shape == "c":
        # Trace the full C as a single polygon: header, inner cavity and closing arm.
        p.lineTo(r, 48); p.lineTo(43, 48); p.lineTo(43, 52); p.lineTo(38, 52)
        p.lineTo(38, b - 40); p.lineTo(43, b - 40); p.lineTo(43, b - 35)
        p.lineTo(r, b - 35); p.lineTo(r, b - 5); p.lineTo(r - 2, b - 5)
        p.lineTo(r - 2, b - 2); p.lineTo(r - 5, b - 2); p.lineTo(r - 5, b)
        _bottom_connector(p, b); _bottom_left_corner(p, b); p.lineTo(3, 8)
        p.lineTo(5, 8); p.lineTo(5, 5); p.lineTo(8, 5); p.lineTo(8, 3)
    else:
        p.lineTo(r, b - 5); p.lineTo(r - 2, b - 5); p.lineTo(r - 2, b - 2)
        p.lineTo(r - 5, b - 2); p.lineTo(r - 5, b)
        _bottom_connector(p, b); _bottom_left_corner(p, b)
        p.lineTo(3, 20 if shape == "hat" else 8)
        if shape != "hat":
            p.lineTo(5, 8); p.lineTo(5, 5); p.lineTo(8, 5); p.lineTo(8, 3)
    p.closeSubpath()
    return p

def highlight_path(width, shape):
    """Inset highlight follows the real top contour and never crosses a notch/hat."""
    r = width - BODY_RIGHT
    p = QPainterPath()
    if shape == "hat":
        p.moveTo(8, 24); p.lineTo(38, 24); p.lineTo(38, 8)
        p.lineTo(63, 8); p.lineTo(63, 11); p.lineTo(68, 11); p.lineTo(68, 15)
        p.lineTo(73, 15); p.lineTo(73, 19); p.lineTo(78, 19); p.lineTo(78, 23)
        p.lineTo(83, 23); p.lineTo(83, 27); p.lineTo(88, 27); p.lineTo(88, 29); p.lineTo(r - 5, 29)
    elif shape == "reporter":
        p.moveTo(15, 7); p.lineTo(r - 12, 7)
    else:
        p.moveTo(7, 7); p.lineTo(NOTCH_X - 2, 7)
        p.moveTo(NOTCH_X + NOTCH_W + 2, 7); p.lineTo(r - 5, 7)
    return p

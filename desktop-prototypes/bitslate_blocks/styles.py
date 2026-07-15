from pathlib import Path
from PySide6.QtGui import QColor, QFont, QFontDatabase

_FONT_PATH = Path(__file__).resolve().parent / "assets" / "fonts" / "monogram.ttf"
_MONOGRAM_FAMILY = None
_FONT_LOAD_ATTEMPTED = False

CATEGORIES = {
    # Monogram-supported pixel symbols avoid platform font substitution while
    # keeping each category visually distinct.
    "movement": ("Movement", "+", QColor("#4d69b2")),
    "events": ("Events", "!", QColor("#e3b43e")),
    "control": ("Control", "{}", QColor("#ce762d")),
    "operators": ("Operators", "+ - x /", QColor("#65983b")),
    "camera": ("Camera", "[o]", QColor("#68459d")),
}

def load_monogram_font():
    """Register the bundled font and use Qt's reported internal family name."""
    global _FONT_LOAD_ATTEMPTED, _MONOGRAM_FAMILY
    if _FONT_LOAD_ATTEMPTED:
        return _MONOGRAM_FAMILY
    _FONT_LOAD_ATTEMPTED = True
    font_id = QFontDatabase.addApplicationFont(str(_FONT_PATH))
    if font_id >= 0:
        families = QFontDatabase.applicationFontFamilies(font_id)
        if families:
            _MONOGRAM_FAMILY = families[0]
    return _MONOGRAM_FAMILY

def pixel_font(size=11, bold=True):
    family = load_monogram_font()
    if not family:
        families = set(QFontDatabase.families())
        family = next((f for f in ("Consolas", "Courier New", "DejaVu Sans Mono") if f in families), "Monospace")
    font = QFont(family)
    # Monogram's native glyph grid is intentionally small; integer 2x scaling
    # preserves its crisp pixel proportions at normal desktop DPI.
    font.setPixelSize(int(size * 2))
    font.setBold(bool(bold and not _MONOGRAM_FAMILY))
    font.setStyleHint(QFont.StyleHint.Monospace)
    font.setHintingPreference(QFont.HintingPreference.PreferFullHinting)
    return font

def darken(color, amount=155):
    return color.darker(amount)

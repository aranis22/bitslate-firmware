# BitSlate block editor prototype

A standalone, programmatically rendered PySide6 block-coding editor. No web runtime or image assets are used.

The interface bundles Datagoblin's Monogram pixel font under `assets/fonts` and registers it directly with Qt at startup, so no system font installation is required. Its upstream license is included beside the font file.

## Install and launch

```powershell
cd desktop-prototypes\bitslate_blocks
py -3 -m venv .venv
.venv\Scripts\Activate.ps1
python -m pip install -r requirements.txt
python main.py
```

Drag palette blocks onto the white canvas. Select and drag workspace blocks to move them; nearby vertical connectors snap together. Delete or Backspace removes selected blocks. Hold Space or the middle mouse button to pan. Hold Ctrl while using the wheel to zoom, or use the lower-left zoom controls.

The renderer uses reusable definitions, separate serializable models, painter paths, and editable `QLineEdit` fields embedded in graphics items. C-shaped blocks accept visual placement in their cavity in this prototype; nested execution is intentionally out of scope.

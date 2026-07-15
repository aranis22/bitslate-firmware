import sys
from PySide6.QtWidgets import QApplication
from editor_window import EditorWindow
from styles import load_monogram_font

def main():
    app=QApplication(sys.argv); app.setApplicationName("BitSlate"); load_monogram_font(); window=EditorWindow(); window.show(); return app.exec()

if __name__ == "__main__": raise SystemExit(main())

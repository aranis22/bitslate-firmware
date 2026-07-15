from PySide6.QtCore import Qt
import random
from PySide6.QtWidgets import QMainWindow, QWidget, QHBoxLayout, QVBoxLayout, QPushButton, QMessageBox, QLabel, QStackedWidget
from workspace import WorkspaceView
from palette import Palette
from styles import pixel_font
from sprite_selector import SpriteSelectorPage, BackdropSelectorPage

class EditorWindow(QMainWindow):
    def __init__(self):
        super().__init__(); self.setWindowTitle("BitSlate Block Editor"); self.resize(1280,800)
        central=QWidget();self.editor_page=central;outer=QHBoxLayout(central);self.editor_layout=outer;outer.setContentsMargins(10,10,10,10)
        left=QWidget(); layout=QVBoxLayout(left); layout.setContentsMargins(0,0,0,0)
        heading=QLabel("BITSLATE • CODE WORKSPACE"); heading.setFont(pixel_font(12)); heading.setStyleSheet("color:#27324b;padding:8px 6px;"); layout.addWidget(heading)
        self.view=WorkspaceView(); layout.addWidget(self.view,1)
        tools=QHBoxLayout()
        for text,fn in (("+",lambda:self.set_zoom(1.2)),("−",lambda:self.set_zoom(1/1.2))):
            b=QPushButton(text); b.setObjectName("workspaceTool"); b.setFont(pixel_font(11)); b.clicked.connect(fn); tools.addWidget(b)
        self.zoom_label=QLabel("100%");self.zoom_label.setFont(pixel_font(10));self.zoom_label.setStyleSheet("color:#394050;padding:6px;");tools.addWidget(self.zoom_label)
        reset=QPushButton("Reset");reset.setObjectName("workspaceTool");reset.setFont(pixel_font(10));reset.clicked.connect(self.reset_zoom);tools.addWidget(reset)
        tools.addStretch(); clear=QPushButton("Clear Workspace");clear.setObjectName("workspaceTool");clear.setFont(pixel_font(10));clear.clicked.connect(self.clear_workspace);tools.addWidget(clear);layout.addLayout(tools)
        # Permanent default: compact tools/sidebar left, 72% code workspace right.
        self.palette=Palette();outer.addWidget(self.palette,7);outer.addWidget(left,18)
        self.sprite_selector=SpriteSelectorPage();self.backdrop_selector=BackdropSelectorPage();self.pages=QStackedWidget();self.pages.addWidget(self.editor_page);self.pages.addWidget(self.sprite_selector);self.pages.addWidget(self.backdrop_selector);self.setCentralWidget(self.pages)
        sprite_toolbar=self.palette.output_preview.canvas.toolbars.left_toolbar
        sprite_toolbar.buttons[0].clicked.connect(self.randomize_sprite)
        # Paint (index 1) intentionally remains a clickable placeholder.
        sprite_toolbar.buttons[2].clicked.connect(self.open_sprite_selector)
        self.sprite_selector.backRequested.connect(self.close_sprite_selector);self.sprite_selector.confirmed.connect(self.confirm_sprite)
        backdrop_toolbar=self.palette.output_preview.canvas.toolbars.right_toolbar
        backdrop_toolbar.buttons[0].clicked.connect(self.randomize_backdrop)
        # Backdrop Paint (index 1) intentionally remains a placeholder.
        backdrop_toolbar.buttons[2].clicked.connect(self.open_backdrop_selector)
        self.backdrop_selector.backRequested.connect(self.close_backdrop_selector);self.backdrop_selector.confirmed.connect(self.confirm_backdrop)
        default=next((p for p in self.sprite_selector.sprite_paths if p.name.lower()=="capybara.png"),self.sprite_selector.sprite_paths[0] if self.sprite_selector.sprite_paths else None)
        self.current_sprite_path=str(default) if default else None
        self.palette.output_preview.canvas.set_sprite(default);self.sprite_selector.set_current_path(default)
        default_backdrop=next((p for p in self.backdrop_selector.sprite_paths if p.name.lower()=="wilderness.png"),self.backdrop_selector.sprite_paths[0] if self.backdrop_selector.sprite_paths else None)
        self.current_backdrop_path=str(default_backdrop) if default_backdrop else None
        self.palette.output_preview.canvas.set_backdrop(default_backdrop);self.backdrop_selector.set_current_path(default_backdrop)
        self.setStyleSheet("QMainWindow{background:#d9dbe3;} QPushButton#workspaceTool{color:#000;background:#f5f5f5;border:3px solid #000;padding:7px;} QPushButton#workspaceTool:hover{background:#fff4ad;} QPushButton#workspaceTool:pressed{background:#ded28e;} QPushButton#workspaceTool:disabled{color:#222;background:#d8d8d8;border-color:#000;}")
    def clear_workspace(self):
        if QMessageBox.question(self,"Clear Workspace","Delete every block from the workspace?",QMessageBox.StandardButton.Yes|QMessageBox.StandardButton.No)==QMessageBox.StandardButton.Yes:self.view.workspace.clear()
    def set_zoom(self,factor):
        self.view.scale_by(factor);self.zoom_label.setText(f"{round(self.view.transform().m11()*100)}%")
    def reset_zoom(self):
        self.view.reset_zoom();self.zoom_label.setText("100%")
    def open_sprite_selector(self):
        self.sprite_selector.set_current_path(self.current_sprite_path);self.pages.setCurrentWidget(self.sprite_selector);self.sprite_selector.setFocus()
    def close_sprite_selector(self):
        self.pages.setCurrentWidget(self.editor_page)
    def confirm_sprite(self,path):
        self.current_sprite_path=path;self.palette.output_preview.canvas.set_sprite(path);self.close_sprite_selector()
    def randomize_sprite(self):
        if not self.sprite_selector.sprite_paths:return
        path=random.choice(self.sprite_selector.sprite_paths);self.current_sprite_path=str(path);self.palette.output_preview.canvas.set_sprite(path);self.sprite_selector.set_current_path(path)
    def open_backdrop_selector(self):
        self.backdrop_selector.set_current_path(self.current_backdrop_path);self.pages.setCurrentWidget(self.backdrop_selector);self.backdrop_selector.setFocus()
    def close_backdrop_selector(self):
        self.pages.setCurrentWidget(self.editor_page)
    def confirm_backdrop(self,path):
        self.current_backdrop_path=path;self.palette.output_preview.canvas.set_backdrop(path);self.close_backdrop_selector()
    def randomize_backdrop(self):
        if not self.backdrop_selector.sprite_paths:return
        path=random.choice(self.backdrop_selector.sprite_paths);self.current_backdrop_path=str(path);self.palette.output_preview.canvas.set_backdrop(path);self.backdrop_selector.set_current_path(path)

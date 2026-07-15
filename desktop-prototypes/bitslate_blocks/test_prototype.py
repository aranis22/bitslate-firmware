import os
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtCore import Qt, QPointF
from PySide6.QtWidgets import QApplication, QLabel, QPushButton
from block_items import BlockItem
from block_models import BLOCKS, DEFINITIONS
from editor_window import EditorWindow
from block_shapes import block_path
from styles import load_monogram_font, pixel_font
from palette import PaletteBlock, OutputPreview
from preview_tools import PixelIconButton, ASSET_DIR

app = QApplication.instance() or QApplication([])

def test_bundled_monogram_font_loads():
    family = load_monogram_font()
    assert family
    assert pixel_font().family() == family

def test_catalog_and_models():
    assert set(BLOCKS) == {"movement", "events", "control", "operators", "camera"}
    assert len(DEFINITIONS) == 16
    window = EditorWindow()
    item = window.view.workspace.add_block("control_wait", QPointF(120, 80))
    assert item.model.to_dict()["arguments"]["seconds"] == "0.5"

def test_snapping_links_models_and_motion():
    window = EditorWindow(); scene = window.view.workspace
    top = scene.add_block("motion_move", QPointF(100, 100))
    lower = scene.add_block("control_wait", QPointF(100, 150))
    scene.try_snap(lower)
    assert top.model.next_block == lower.model.id
    assert lower.model.parent_block == top.model.id
    old = lower.pos(); top.setPos(top.pos() + QPointF(25, 10))
    assert lower.pos() == old + QPointF(25, 10)

def test_camera_empty_state():
    window = EditorWindow(); palette = window.palette
    palette.show_category("camera")
    labels = palette.scroll.widget().findChildren(QLabel)
    assert any(label.text() == "No camera blocks yet." for label in labels)

def test_every_silhouette_stays_in_bounds():
    window = EditorWindow()
    for definition in DEFINITIONS.values():
        item = window.view.workspace.add_block(definition.opcode, QPointF())
        bounds = item.boundingRect()
        assert bounds.contains(block_path(item.width, item.height, definition.shape).boundingRect())

def test_shared_input_vertical_offset():
    window = EditorWindow()
    for opcode in ("motion_move", "motion_turn", "control_wait", "control_repeat"):
        item = window.view.workspace.add_block(opcode, QPointF())
        expected_y = (18 if item.definition.shape == "hat" else 13) - item.INPUT_RAISE
        assert all(proxy.pos().y() == expected_y for _, _, proxy, _ in item.inputs)

def test_drag_preview_background_is_transparent():
    preview = PaletteBlock(BLOCKS["control"][0])
    pixmap = preview.drag_pixmap()
    assert pixmap.hasAlphaChannel()
    image = pixmap.toImage()
    for x, y in ((0, 0), (pixmap.width()-1, 0), (0, pixmap.height()-1),
                 (pixmap.width()-1, pixmap.height()-1)):
        assert image.pixelColor(x, y).alpha() == 0

def test_run_controls_and_start_event_icon():
    window = EditorWindow()
    controls = window.view.run_controls
    assert controls.parentWidget() is window.view.viewport()
    assert controls.testAttribute(Qt.WidgetAttribute.WA_TransparentForMouseEvents)
    start = window.view.workspace.add_block("event_start", QPointF())
    assert any(kind == "play_icon" for kind, *_ in start._layout)

def test_compact_sidebar_selector_and_preview():
    window = EditorWindow();palette = window.palette
    assert palette.header.isVisibleTo(palette)
    assert not palette.dropdown.isVisible()
    assert palette.splitter.count() == 2
    assert isinstance(palette.output_preview, OutputPreview)
    palette.show_category("control")
    assert palette.current_category == "control"
    assert "CONTROL" in palette.header.text()

def test_palette_receives_remaining_sidebar_height():
    window = EditorWindow();window.resize(1280,800);window.show();app.processEvents()
    palette = window.palette;sizes=palette.splitter.sizes()
    assert sizes[0] > sizes[1]
    canvas=palette.output_preview.canvas;host=palette.output_preview.preview_host
    assert canvas.y()+canvas.height() == host.height()
    assert canvas.width()*2 == canvas.height()*3

def test_permanent_sidebar_left_and_workspace_button_style():
    window = EditorWindow();layout = window.editor_layout
    assert hasattr(layout.itemAt(0).widget(), "current_category")
    assert layout.stretch(0) == 7 and layout.stretch(1) == 18
    buttons = window.findChildren(QPushButton, "workspaceTool")
    assert len(buttons) == 4
    assert all(button.font().family() == load_monogram_font() for button in buttons)
    assert "color:#000" in window.styleSheet()

def test_output_preview_pixel_toolbars():
    window = EditorWindow();window.resize(1200,760);window.show();app.processEvents()
    palette = window.palette;canvas = palette.output_preview.canvas
    buttons = canvas.findChildren(PixelIconButton)
    assert len(buttons) == 8
    assert {button.filename for button in buttons} == {"sparkle.png","paint.png","search.png","capybara_plus.png","image.png"}
    assert all((ASSET_DIR/button.filename).is_file() for button in buttons)
    pair = canvas.toolbars
    assert canvas.width()*2 == canvas.height()*3
    assert canvas.width() <= 480 and canvas.height() <= 320
    assert pair.height() == 51
    assert not pair.left_toolbar.expanded and not pair.right_toolbar.expanded
    assert all(not button.isVisible() for button in pair.left_toolbar.buttons[:-1])
    assert pair.x() >= 0 and pair.y() >= 0
    assert pair.x()+pair.width() <= canvas.width() and pair.y()+pair.height() <= canvas.height()
    bottom = pair.y()+pair.height()
    pair.right_toolbar.trigger.click();app.processEvents()
    assert pair.right_toolbar.expanded and not pair.left_toolbar.expanded
    assert pair.height() == 190 and pair.y()+pair.height() == bottom
    assert all(button.isVisible() for button in pair.right_toolbar.buttons[:-1])
    pair.right_toolbar.trigger.click();app.processEvents()
    assert pair.height() == 51 and not pair.right_toolbar.expanded

def test_sprite_selector_pages_and_preserves_editor_state():
    window=EditorWindow();window.resize(1280,800);window.show();app.processEvents()
    block=window.view.workspace.add_block("motion_move",QPointF(100,100));selector=window.sprite_selector
    assert len(selector.sprite_paths)==12 and selector.page_count==3
    toolbar=window.palette.output_preview.canvas.toolbars.left_toolbar
    toolbar.trigger.click();app.processEvents()
    assert toolbar.expanded and window.pages.currentWidget() is window.editor_page
    toolbar.buttons[2].click();app.processEvents()
    assert window.pages.currentWidget() is selector
    assert sum(card.isVisible() for card in selector.cards)==4
    selector.set_page(1);assert selector.page==1
    chosen=str(selector.sprite_paths[4]);selector.select_path(chosen);selector.confirm_selection();app.processEvents()
    assert window.current_sprite_path==chosen and window.pages.currentWidget() is window.editor_page
    assert block in window.view.workspace.items()
    window.open_sprite_selector();selector.select_path(str(selector.sprite_paths[0]));window.close_sprite_selector()
    assert window.current_sprite_path==chosen

def test_default_and_random_sprites_update_preview():
    window=EditorWindow();canvas=window.palette.output_preview.canvas
    assert window.current_sprite_path and window.current_sprite_path.lower().endswith("capybara.png")
    assert canvas.current_sprite_path==window.current_sprite_path and not canvas.sprite_pixmap.isNull()
    window.randomize_sprite()
    assert canvas.current_sprite_path==window.current_sprite_path
    assert any(str(path)==window.current_sprite_path for path in window.sprite_selector.sprite_paths)

def test_backdrop_workflow_and_layered_preview():
    window=EditorWindow();window.resize(1280,800);window.show();app.processEvents();canvas=window.palette.output_preview.canvas
    assert len(window.backdrop_selector.sprite_paths)==11 and window.backdrop_selector.page_count==3
    assert window.current_backdrop_path.lower().endswith("wilderness.png")
    assert canvas.current_backdrop_path==window.current_backdrop_path and not canvas.backdrop_pixmap.isNull()
    original_sprite=window.current_sprite_path;toolbar=canvas.toolbars.right_toolbar
    toolbar.trigger.click();app.processEvents();assert toolbar.expanded and window.pages.currentWidget() is window.editor_page
    toolbar.buttons[2].click();app.processEvents();assert window.pages.currentWidget() is window.backdrop_selector
    chosen=str(window.backdrop_selector.sprite_paths[1]);window.backdrop_selector.select_path(chosen);window.backdrop_selector.confirm_selection();app.processEvents()
    assert window.current_backdrop_path==chosen and canvas.current_backdrop_path==chosen
    assert window.current_sprite_path==original_sprite and window.pages.currentWidget() is window.editor_page
    window.randomize_backdrop();assert canvas.current_backdrop_path==window.current_backdrop_path

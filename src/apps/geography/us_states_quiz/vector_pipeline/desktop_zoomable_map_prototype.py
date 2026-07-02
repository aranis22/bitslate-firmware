from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
import sys
import tkinter as tk
from typing import Sequence

import shapefile

try:
    from PIL import Image, ImageDraw
except ImportError:
    Image = None
    ImageDraw = None


ROOT = Path(__file__).resolve().parents[5]
PIPELINE_DIR = Path(__file__).resolve().parent
DATA_DIR = PIPELINE_DIR / "data"
ARTIFACT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "artifacts"
PREVIEW_PATH = ARTIFACT_DIR / "desktop_zoomable_map_labels_preview.png"

WINDOW_W = 1000
WINDOW_H = 650
TOP_H = 72
CANVAS_W = WINDOW_W
CANVAS_H = WINDOW_H - TOP_H
PAD_X = 36
PAD_Y = 28

BACKGROUND = "#eef2f7"
STATE_PALETTE = [
    "#9fb58a",
    "#c5a25b",
    "#8fa8be",
    "#b98986",
    "#a99bbd",
    "#97b2aa",
    "#c8bd82",
    "#9aab7c",
    "#b8a2a8",
    "#8e9fc6",
    "#b5b1a4",
    "#c1a076",
]
BORDER = "#f4f4f0"
BIT_GRID = "#3f4645"
TEXT = "#172033"
STATUS = "#334155"
CLICKED_OUTLINE = "#111827"
CAPITAL_FILL = "#101418"
CAPITAL_OUTLINE = "#f8fafc"
LABEL_TEXT = "#101418"
LABEL_SHADOW = "#f8fafc"

MIN_ZOOM = 1.0
MAX_ZOOM = 10.0
WHEEL_FACTOR = 1.18
KEY_FACTOR = 1.25
DRAG_CLICK_THRESHOLD = 4
BIT_CELL_SIZE = 8.0
STATE_LABEL_ZOOM = 1.7
CAPITAL_LABEL_ZOOM = 2.0

EXCLUDED_STUSPS = {
    "AK",
    "HI",
    "PR",
    "GU",
    "VI",
    "AS",
    "MP",
    "DC",
}

Point = tuple[float, float]
Ring = list[Point]


@dataclass
class StateShape:
    name: str
    stusps: str
    statefp: str
    source_rings: list[Ring]
    world_rings: list[Ring]
    bit_cells: list[tuple[float, float, float, float]] = field(default_factory=list)
    label_point: Point = (0.0, 0.0)


@dataclass
class CapitalMarker:
    state: str
    capital: str
    lon: float
    lat: float
    world_x: float = 0.0
    world_y: float = 0.0


CAPITALS: list[CapitalMarker] = [
    CapitalMarker("New York", "Albany", -73.7562, 42.6526),
    CapitalMarker("Texas", "Austin", -97.7431, 30.2672),
    CapitalMarker("Wyoming", "Cheyenne", -104.8202, 41.1400),
    CapitalMarker("Kansas", "Topeka", -95.6890, 39.0473),
    CapitalMarker("New Jersey", "Trenton", -74.7429, 40.2206),
    CapitalMarker("Idaho", "Boise", -116.2023, 43.6150),
    CapitalMarker("Wisconsin", "Madison", -89.4012, 43.0731),
    CapitalMarker("Connecticut", "Hartford", -72.6851, 41.7637),
    CapitalMarker("Michigan", "Lansing", -84.5555, 42.7325),
    CapitalMarker("Montana", "Helena", -112.0391, 46.5891),
    CapitalMarker("Arizona", "Phoenix", -112.0740, 33.4484),
    CapitalMarker("New Mexico", "Santa Fe", -105.9378, 35.6870),
    CapitalMarker("Oregon", "Salem", -123.0351, 44.9429),
    CapitalMarker("Washington", "Olympia", -122.9007, 47.0379),
    CapitalMarker("California", "Sacramento", -121.4944, 38.5816),
    CapitalMarker("Tennessee", "Nashville", -86.7816, 36.1627),
]


def find_shapefile() -> Path | None:
    expected = DATA_DIR / "cb_2024_us_state_20m" / "cb_2024_us_state_20m.shp"
    if expected.exists():
        return expected
    candidates = sorted(DATA_DIR.rglob("*.shp"))
    return candidates[0] if candidates else None


def shape_to_rings(shape: shapefile.Shape) -> list[Ring]:
    points = shape.points
    parts = list(shape.parts) + [len(points)]
    rings: list[Ring] = []
    for index in range(len(parts) - 1):
        ring = [(float(x), float(y)) for x, y in points[parts[index] : parts[index + 1]]]
        if len(ring) >= 3:
            rings.append(ring)
    return rings


def load_states(shapefile_path: Path) -> tuple[list[StateShape], int]:
    reader = shapefile.Reader(str(shapefile_path))
    fields = [field[0] for field in reader.fields[1:]]
    all_count = 0
    states: list[StateShape] = []

    for shape_record in reader.iterShapeRecords():
        all_count += 1
        record = dict(zip(fields, shape_record.record))
        stusps = str(record.get("STUSPS", ""))
        if stusps in EXCLUDED_STUSPS:
            continue

        source_rings = shape_to_rings(shape_record.shape)
        if not source_rings:
            continue

        states.append(
            StateShape(
                name=str(record.get("NAME", stusps)),
                stusps=stusps,
                statefp=str(record.get("STATEFP", "")),
                source_rings=source_rings,
                world_rings=[],
            )
        )

    project_states_to_world(states)
    return states, all_count


def source_bounds(states: Sequence[StateShape]) -> tuple[float, float, float, float]:
    xs = [x for state in states for ring in state.source_rings for x, _ in ring]
    ys = [y for state in states for ring in state.source_rings for _, y in ring]
    return min(xs), min(ys), max(xs), max(ys)


def project_states_to_world(states: Sequence[StateShape]) -> None:
    min_x, min_y, max_x, max_y = source_bounds(states)
    map_w = CANVAS_W - PAD_X * 2
    map_h = CANVAS_H - PAD_Y * 2
    scale = min(map_w / (max_x - min_x), map_h / (max_y - min_y))
    drawn_w = (max_x - min_x) * scale
    drawn_h = (max_y - min_y) * scale
    offset_x = (CANVAS_W - drawn_w) / 2
    offset_y = (CANVAS_H - drawn_h) / 2

    def project(point: Point) -> Point:
        x, y = point
        return offset_x + (x - min_x) * scale, offset_y + drawn_h - (y - min_y) * scale

    for state in states:
        state.world_rings = [[project(point) for point in ring] for ring in state.source_rings]
        state.bit_cells = build_state_bit_cells(state)
        state.label_point = find_state_label_point(state)

    for marker in CAPITALS:
        marker.world_x, marker.world_y = project((marker.lon, marker.lat))


def flatten_ring(ring: Ring) -> list[float]:
    coords: list[float] = []
    for x, y in ring:
        coords.extend((x, y))
    return coords


def muted_state_color(name: str) -> str:
    palette_index = sum((index + 1) * ord(char) for index, char in enumerate(name)) % len(STATE_PALETTE)
    return STATE_PALETTE[palette_index]


def point_in_ring(point: Point, ring: Ring) -> bool:
    x, y = point
    inside = False
    j = len(ring) - 1
    for i, (xi, yi) in enumerate(ring):
        xj, yj = ring[j]
        if (yi > y) != (yj > y):
            cross_x = (xj - xi) * (y - yi) / ((yj - yi) or 1e-12) + xi
            if x < cross_x:
                inside = not inside
        j = i
    return inside


def point_in_state(point: Point, state: StateShape) -> bool:
    return any(point_in_ring(point, ring) for ring in state.world_rings)


def state_center(state: StateShape) -> Point:
    points = [point for ring in state.world_rings for point in ring]
    return sum(x for x, _ in points) / len(points), sum(y for _, y in points) / len(points)


def state_world_bounds(state: StateShape) -> tuple[float, float, float, float]:
    xs = [x for ring in state.world_rings for x, _ in ring]
    ys = [y for ring in state.world_rings for _, y in ring]
    return min(xs), min(ys), max(xs), max(ys)


def find_state_label_point(state: StateShape) -> Point:
    min_x, min_y, max_x, max_y = state_world_bounds(state)
    center = ((min_x + max_x) / 2, (min_y + max_y) / 2)
    if point_in_state(center, state):
        return center

    points = [point for ring in state.world_rings for point in ring]
    return min(
        points,
        key=lambda point: (point[0] - center[0]) * (point[0] - center[0])
        + (point[1] - center[1]) * (point[1] - center[1]),
    )


def build_state_bit_cells(state: StateShape) -> list[tuple[float, float, float, float]]:
    min_x, min_y, max_x, max_y = state_world_bounds(state)
    start_x = int(min_x // BIT_CELL_SIZE) * BIT_CELL_SIZE
    start_y = int(min_y // BIT_CELL_SIZE) * BIT_CELL_SIZE
    cells: list[tuple[float, float, float, float]] = []
    y = start_y
    while y <= max_y:
        x = start_x
        while x <= max_x:
            center = (x + BIT_CELL_SIZE / 2, y + BIT_CELL_SIZE / 2)
            if point_in_state(center, state):
                cells.append((x, y, x + BIT_CELL_SIZE, y + BIT_CELL_SIZE))
            x += BIT_CELL_SIZE
        y += BIT_CELL_SIZE
    return cells


def iter_state_bit_cells(state: StateShape) -> Sequence[tuple[float, float, float, float]]:
    return state.bit_cells


def preview_world_to_screen(wx: float, wy: float, zoom: float = 2.05) -> Point:
    sx = (wx - CANVAS_W / 2) * zoom + CANVAS_W / 2
    sy = (wy - CANVAS_H / 2) * zoom + CANVAS_H / 2
    return sx, sy


def draw_preview_text(draw: ImageDraw.ImageDraw, sx: float, sy: float, text: str, centered: bool = False) -> None:
    if centered:
        left, top, right, bottom = draw.textbbox((0, 0), text)
        sx -= (right - left) / 2
        sy -= bottom - top
    draw.text((sx + 1, sy + 1), text, fill=LABEL_SHADOW)
    draw.text((sx - 1, sy), text, fill=LABEL_SHADOW)
    draw.text((sx, sy), text, fill=LABEL_TEXT)


def save_label_preview(states: list[StateShape]) -> bool:
    if Image is None or ImageDraw is None:
        return False

    image = Image.new("RGB", (CANVAS_W, CANVAS_H), BACKGROUND)
    draw = ImageDraw.Draw(image)

    def transform_ring(ring: Ring) -> Ring:
        return [preview_world_to_screen(x, y) for x, y in ring]

    for state in states:
        for ring in state.world_rings:
            draw.polygon(transform_ring(ring), fill=muted_state_color(state.name), outline=BORDER)
    for state in states:
        for x1, y1, x2, y2 in iter_state_bit_cells(state):
            sx1, sy1 = preview_world_to_screen(x1, y1)
            sx2, sy2 = preview_world_to_screen(x2, y2)
            draw.rectangle((sx1, sy1, sx2, sy2), outline=BIT_GRID)
    for state in states:
        for ring in state.world_rings:
            transformed = transform_ring(ring + [ring[0]])
            draw.line(transformed, fill=BORDER, width=1)
    for marker in CAPITALS:
        sx, sy = preview_world_to_screen(marker.world_x, marker.world_y)
        radius = 4
        draw.ellipse((sx - radius, sy - radius, sx + radius, sy + radius), fill=CAPITAL_FILL, outline=CAPITAL_OUTLINE, width=2)
    for state in states:
        sx, sy = preview_world_to_screen(*state.label_point)
        if -80 <= sx <= CANVAS_W + 80 and -24 <= sy <= CANVAS_H + 24:
            draw_preview_text(draw, sx, sy, state.name)
    for marker in CAPITALS:
        sx, sy = preview_world_to_screen(marker.world_x, marker.world_y)
        if -80 <= sx <= CANVAS_W + 80 and -24 <= sy <= CANVAS_H + 24:
            draw_preview_text(draw, sx, sy - 12, marker.capital, centered=True)

    ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    image.save(PREVIEW_PATH)
    return True


class ZoomableMapApp:
    def __init__(self, states: list[StateShape], shapefile_path: Path, all_state_count: int) -> None:
        self.states = states
        self.shapefile_path = shapefile_path
        self.all_state_count = all_state_count
        self.camera_center_x = CANVAS_W / 2
        self.camera_center_y = CANVAS_H / 2
        self.zoom = 1.0
        self.clicked_state = ""
        self.click_message = "Click a state"
        self.show_grid = True
        self.show_capitals = True
        self.show_labels = True
        self.drag_start: Point | None = None
        self.drag_last: Point | None = None
        self.drag_total = 0.0

        self.root = tk.Tk()
        self.root.title("BitSlate Zoomable US Map Prototype")
        self.root.geometry(f"{WINDOW_W}x{WINDOW_H}")
        self.root.configure(bg=BACKGROUND)
        self.root.bind("<Escape>", lambda _event: self.root.destroy())
        self.root.bind("r", lambda _event: self.reset_camera())
        self.root.bind("R", lambda _event: self.reset_camera())
        self.root.bind("+", lambda _event: self.zoom_about_center(KEY_FACTOR))
        self.root.bind("=", lambda _event: self.zoom_about_center(KEY_FACTOR))
        self.root.bind("-", lambda _event: self.zoom_about_center(1 / KEY_FACTOR))
        self.root.bind("_", lambda _event: self.zoom_about_center(1 / KEY_FACTOR))
        self.root.bind("g", lambda _event: self.toggle_grid())
        self.root.bind("G", lambda _event: self.toggle_grid())
        self.root.bind("c", lambda _event: self.toggle_capitals())
        self.root.bind("C", lambda _event: self.toggle_capitals())
        self.root.bind("l", lambda _event: self.toggle_labels())
        self.root.bind("L", lambda _event: self.toggle_labels())

        self.prompt = tk.StringVar()
        self.status = tk.StringVar()

        top = tk.Frame(self.root, bg=BACKGROUND, height=TOP_H)
        top.pack(fill=tk.X)
        top.pack_propagate(False)
        tk.Label(
            top,
            textvariable=self.prompt,
            font=("Segoe UI", 20, "bold"),
            fg=TEXT,
            bg=BACKGROUND,
        ).pack(side=tk.LEFT, padx=18, pady=(10, 0))
        tk.Label(
            top,
            textvariable=self.status,
            font=("Segoe UI", 11),
            fg=STATUS,
            bg=BACKGROUND,
        ).pack(side=tk.RIGHT, padx=18, pady=(15, 0))

        self.canvas = tk.Canvas(self.root, width=CANVAS_W, height=CANVAS_H, bg=BACKGROUND, highlightthickness=0)
        self.canvas.pack(fill=tk.BOTH, expand=False)
        self.canvas.bind("<ButtonPress-1>", self.handle_press)
        self.canvas.bind("<B1-Motion>", self.handle_drag)
        self.canvas.bind("<ButtonRelease-1>", self.handle_release)
        self.canvas.bind("<MouseWheel>", self.handle_mousewheel)
        self.canvas.bind("<Button-4>", lambda event: self.zoom_about(event.x, event.y, WHEEL_FACTOR))
        self.canvas.bind("<Button-5>", lambda event: self.zoom_about(event.x, event.y, 1 / WHEEL_FACTOR))

        self.draw()

    def world_to_screen(self, wx: float, wy: float) -> Point:
        sx = (wx - self.camera_center_x) * self.zoom + CANVAS_W / 2
        sy = (wy - self.camera_center_y) * self.zoom + CANVAS_H / 2
        return sx, sy

    def screen_to_world(self, sx: float, sy: float) -> Point:
        wx = (sx - CANVAS_W / 2) / self.zoom + self.camera_center_x
        wy = (sy - CANVAS_H / 2) / self.zoom + self.camera_center_y
        return wx, wy

    def screen_ring(self, ring: Ring) -> Ring:
        return [self.world_to_screen(x, y) for x, y in ring]

    def draw(self) -> None:
        self.canvas.delete("all")
        for state in self.states:
            fill = muted_state_color(state.name)
            outline = CLICKED_OUTLINE if state.name == self.clicked_state else BORDER
            width = 2 if state.name == self.clicked_state else 1
            for ring in state.world_rings:
                screen_points = self.screen_ring(ring)
                self.canvas.create_polygon(
                    flatten_ring(screen_points),
                    fill=fill,
                    outline=outline,
                    width=width,
                    joinstyle=tk.ROUND,
                )

        if self.show_grid:
            self.draw_bit_grid()

        for state in self.states:
            outline = CLICKED_OUTLINE if state.name == self.clicked_state else BORDER
            width = 2 if state.name == self.clicked_state else 1
            for ring in state.world_rings:
                self.canvas.create_line(
                    flatten_ring(self.screen_ring(ring + [ring[0]])),
                    fill=outline,
                    width=width,
                    joinstyle=tk.ROUND,
                )

        if self.show_capitals:
            self.draw_capitals()

        if self.show_labels:
            self.draw_labels()

        self.prompt.set("Zoomable USA Map")
        self.status.set(
            f"{self.click_message}   zoom {self.zoom:.2f}x   grid {'on' if self.show_grid else 'off'}   capitals {'on' if self.show_capitals else 'off'}   labels {'on' if self.show_labels else 'off'}"
        )
        self.canvas.create_text(
            12,
            CANVAS_H - 12,
            text="Wheel/+/- zoom   Drag pan   G grid   C capitals   L labels   R reset   Esc quit",
            anchor=tk.SW,
            fill=STATUS,
            font=("Segoe UI", 10),
        )

    def draw_bit_grid(self) -> None:
        for state in self.states:
            for x1, y1, x2, y2 in iter_state_bit_cells(state):
                sx1, sy1 = self.world_to_screen(x1, y1)
                sx2, sy2 = self.world_to_screen(x2, y2)
                self.canvas.create_rectangle(
                    sx1,
                    sy1,
                    sx2,
                    sy2,
                    outline=BIT_GRID,
                    width=1,
                )

    def capital_radius(self) -> float:
        return max(3.0, min(7.0, 3.0 * (self.zoom ** 0.35)))

    def draw_capitals(self) -> None:
        radius = self.capital_radius()
        for marker in CAPITALS:
            sx, sy = self.world_to_screen(marker.world_x, marker.world_y)
            self.canvas.create_oval(
                sx - radius,
                sy - radius,
                sx + radius,
                sy + radius,
                fill=CAPITAL_FILL,
                outline=CAPITAL_OUTLINE,
                width=2,
            )

    def point_on_screen(self, sx: float, sy: float) -> bool:
        return -80 <= sx <= CANVAS_W + 80 and -24 <= sy <= CANVAS_H + 24

    def draw_text_with_shadow(
        self,
        sx: float,
        sy: float,
        text: str,
        font: tuple[str, int, str] | tuple[str, int],
        anchor: str = tk.CENTER,
    ) -> None:
        self.canvas.create_text(sx + 1, sy + 1, text=text, fill=LABEL_SHADOW, font=font, anchor=anchor)
        self.canvas.create_text(sx - 1, sy, text=text, fill=LABEL_SHADOW, font=font, anchor=anchor)
        self.canvas.create_text(sx, sy, text=text, fill=LABEL_TEXT, font=font, anchor=anchor)

    def draw_labels(self) -> None:
        if self.zoom >= STATE_LABEL_ZOOM:
            for state in self.states:
                sx, sy = self.world_to_screen(*state.label_point)
                if self.point_on_screen(sx, sy):
                    self.draw_text_with_shadow(sx, sy, state.name, ("Segoe UI", 8, "bold"))

        if self.zoom >= CAPITAL_LABEL_ZOOM and self.show_capitals:
            radius = self.capital_radius()
            for marker in CAPITALS:
                sx, sy = self.world_to_screen(marker.world_x, marker.world_y)
                if self.point_on_screen(sx, sy):
                    self.draw_text_with_shadow(sx, sy - radius - 9, marker.capital, ("Segoe UI", 8), anchor=tk.S)

    def reset_camera(self) -> None:
        self.camera_center_x = CANVAS_W / 2
        self.camera_center_y = CANVAS_H / 2
        self.zoom = 1.0
        self.click_message = "Reset"
        self.draw()

    def toggle_grid(self) -> None:
        self.show_grid = not self.show_grid
        self.draw()

    def toggle_capitals(self) -> None:
        self.show_capitals = not self.show_capitals
        self.draw()

    def toggle_labels(self) -> None:
        self.show_labels = not self.show_labels
        self.draw()

    def state_at_world(self, point: Point) -> StateShape | None:
        for state in reversed(self.states):
            if point_in_state(point, state):
                return state
        return None

    def capital_at_screen(self, sx: float, sy: float) -> CapitalMarker | None:
        if not self.show_capitals:
            return None
        threshold = self.capital_radius() + 4.0
        threshold_sq = threshold * threshold
        for marker in CAPITALS:
            marker_sx, marker_sy = self.world_to_screen(marker.world_x, marker.world_y)
            dx = sx - marker_sx
            dy = sy - marker_sy
            if dx * dx + dy * dy <= threshold_sq:
                return marker
        return None

    def handle_press(self, event: tk.Event) -> None:
        self.drag_start = (float(event.x), float(event.y))
        self.drag_last = self.drag_start
        self.drag_total = 0.0

    def handle_drag(self, event: tk.Event) -> None:
        if self.drag_last is None:
            return
        last_x, last_y = self.drag_last
        dx = float(event.x) - last_x
        dy = float(event.y) - last_y
        self.camera_center_x -= dx / self.zoom
        self.camera_center_y -= dy / self.zoom
        self.drag_last = (float(event.x), float(event.y))
        self.drag_total += abs(dx) + abs(dy)
        self.draw()

    def handle_release(self, event: tk.Event) -> None:
        if self.drag_total <= DRAG_CLICK_THRESHOLD:
            capital = self.capital_at_screen(float(event.x), float(event.y))
            if capital is not None:
                self.clicked_state = capital.state
                self.click_message = f"Capital: {capital.capital}, {capital.state}"
                print(self.click_message)
                self.draw()
                self.drag_start = None
                self.drag_last = None
                return

            world_point = self.screen_to_world(float(event.x), float(event.y))
            clicked = self.state_at_world(world_point)
            self.clicked_state = clicked.name if clicked is not None else ""
            if clicked is None:
                self.click_message = "Outside map"
                print("Outside map")
            else:
                self.click_message = f"Clicked: {clicked.name}"
                print(f"Clicked: {clicked.name}")
            self.draw()
        self.drag_start = None
        self.drag_last = None

    def handle_mousewheel(self, event: tk.Event) -> None:
        factor = WHEEL_FACTOR if event.delta > 0 else 1 / WHEEL_FACTOR
        self.zoom_about(float(event.x), float(event.y), factor)

    def zoom_about_center(self, factor: float) -> None:
        self.zoom_about(CANVAS_W / 2, CANVAS_H / 2, factor)

    def zoom_about(self, sx: float, sy: float, factor: float) -> None:
        before_x, before_y = self.screen_to_world(sx, sy)
        self.zoom = max(MIN_ZOOM, min(MAX_ZOOM, self.zoom * factor))
        after_x, after_y = self.screen_to_world(sx, sy)
        self.camera_center_x += before_x - after_x
        self.camera_center_y += before_y - after_y
        self.draw()

    def save_preview(self) -> bool:
        return save_label_preview(self.states)

    def run(self) -> None:
        self.root.mainloop()


def save_preview(states: list[StateShape]) -> bool:
    return save_label_preview(states)


def smoke_test(states: list[StateShape]) -> bool:
    by_name = {state.name: state for state in states}
    ok = True
    for name in ("Washington", "California", "Texas", "South Dakota", "Missouri"):
        state = by_name.get(name)
        if state is None:
            print(f"Missing state: {name}")
            ok = False
            continue
        center = state_center(state)
        hit = next((candidate for candidate in states if point_in_state(center, candidate)), None)
        print(f"{name}: center hit {hit.name if hit else 'none'}")
        if hit is None:
            ok = False

    camera_center_x = CANVAS_W / 2
    camera_center_y = CANVAS_H / 2
    zoom = 1.0
    sx = 640.0
    sy = 300.0

    def screen_to_world(sx_value: float, sy_value: float) -> Point:
        wx = (sx_value - CANVAS_W / 2) / zoom + camera_center_x
        wy = (sy_value - CANVAS_H / 2) / zoom + camera_center_y
        return wx, wy

    before = screen_to_world(sx, sy)
    zoom = 2.0
    after = screen_to_world(sx, sy)
    camera_center_x += before[0] - after[0]
    camera_center_y += before[1] - after[1]
    camera_center_x -= 80 / zoom
    camera_center_y -= 30 / zoom
    stabilized = screen_to_world(sx, sy)
    expected_after_pan = before[0] - 40, before[1] - 15
    drift = abs(expected_after_pan[0] - stabilized[0]) + abs(expected_after_pan[1] - stabilized[1])
    texas = by_name.get("Texas")
    texas_hit_after_transform = texas is not None and point_in_state(state_center(texas), texas)
    capital_hits = True
    for marker in CAPITALS:
        state = by_name.get(marker.state)
        if state is None or not point_in_state((marker.world_x, marker.world_y), state):
            print(f"Capital marker outside expected state: {marker.capital}, {marker.state}")
            capital_hits = False
    label_hits = True
    for state in states:
        if not point_in_state(state.label_point, state):
            print(f"State label point outside state: {state.name}")
            label_hits = False
    print(f"Capital markers projected: {len(CAPITALS)}")
    print(f"State label points projected: {len(states)}")
    print(f"Zoom/pan transform drift: {drift:.6f}")
    print(f"Click detection after transform math: {'ok' if texas_hit_after_transform else 'failed'}")
    return ok and drift < 1e-6 and texas_hit_after_transform and capital_hits and label_hits


def main() -> None:
    shapefile_path = find_shapefile()
    if shapefile_path is None:
        print("Missing Census shapefile.")
        print("Run: python src/apps/geography/us_states_quiz/vector_pipeline/import_census_states.py")
        raise SystemExit(1)

    states, all_state_count = load_states(shapefile_path)

    if "--self-test" in sys.argv:
        saved = save_preview(states)
        print(f"Shapefile: {shapefile_path}")
        print(f"Records loaded: {all_state_count}")
        print(f"Contiguous states rendered: {len(states)}")
        print(f"Preview artifact: {PREVIEW_PATH if saved else 'skipped; Pillow unavailable'}")
        raise SystemExit(0 if smoke_test(states) else 1)

    app = ZoomableMapApp(states, shapefile_path, all_state_count)
    saved = app.save_preview()
    print(f"Shapefile: {shapefile_path}")
    print(f"Records loaded: {all_state_count}")
    print(f"Contiguous states rendered: {len(states)}")
    print(f"Preview artifact: {PREVIEW_PATH if saved else 'skipped; Pillow unavailable'}")
    app.run()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)

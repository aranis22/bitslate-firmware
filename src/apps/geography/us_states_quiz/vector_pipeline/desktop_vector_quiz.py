from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
import sys
import tkinter as tk
from typing import Iterable, Sequence

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
PREVIEW_PATH = ARTIFACT_DIR / "census_desktop_vector_quiz_preview.png"

CANVAS_W = 1000
CANVAS_H = 650
TOP_H = 64
BOTTOM_H = 40
PAD_X = 32
PAD_Y = 24

BACKGROUND = "#eef2f7"
MAP_FILL = "#2f3437"
BORDER = "#f7f7f7"
TEXT = "#172033"
STATUS = "#334155"
TARGET_OUTLINE = "#ffd166"
COMPLETED_COLORS = ["#73b86d", "#e89a47", "#b5a34e", "#5e80c9", "#77b45f"]

QUIZ_ORDER = ["Washington", "California", "Texas", "South Dakota", "Missouri"]
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
    screen_rings: list[Ring] = field(default_factory=list)
    canvas_ids: list[int] = field(default_factory=list)


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
        name = str(record.get("NAME", stusps))
        rings = shape_to_rings(shape_record.shape)
        if not rings:
            continue
        states.append(
            StateShape(
                name=name,
                stusps=stusps,
                statefp=str(record.get("STATEFP", "")),
                source_rings=rings,
            )
        )

    return states, all_count


def bounds(states: Sequence[StateShape]) -> tuple[float, float, float, float]:
    xs = [x for state in states for ring in state.source_rings for x, _ in ring]
    ys = [y for state in states for ring in state.source_rings for _, y in ring]
    return min(xs), min(ys), max(xs), max(ys)


def project_states(states: Sequence[StateShape]) -> None:
    min_x, min_y, max_x, max_y = bounds(states)
    map_w = CANVAS_W - PAD_X * 2
    map_h = CANVAS_H - TOP_H - BOTTOM_H - PAD_Y * 2
    scale = min(map_w / (max_x - min_x), map_h / (max_y - min_y))
    drawn_w = (max_x - min_x) * scale
    drawn_h = (max_y - min_y) * scale
    offset_x = (CANVAS_W - drawn_w) / 2
    offset_y = TOP_H + PAD_Y + (map_h - drawn_h) / 2

    def project(point: Point) -> Point:
        x, y = point
        return offset_x + (x - min_x) * scale, offset_y + drawn_h - (y - min_y) * scale

    for state in states:
        state.screen_rings = [[project(point) for point in ring] for ring in state.source_rings]


def flatten_ring(ring: Ring) -> list[float]:
    coords: list[float] = []
    for x, y in ring:
        coords.extend((x, y))
    return coords


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
    return any(point_in_ring(point, ring) for ring in state.screen_rings)


def state_center(state: StateShape) -> Point:
    points = [point for ring in state.screen_rings for point in ring]
    return sum(x for x, _ in points) / len(points), sum(y for _, y in points) / len(points)


class VectorQuizApp:
    def __init__(self, states: list[StateShape], shapefile_path: Path, all_state_count: int) -> None:
        self.states = states
        self.state_by_name = {state.name: state for state in states}
        self.shapefile_path = shapefile_path
        self.all_state_count = all_state_count
        self.current_index = 0
        self.completed: set[str] = set()
        self.last_status = ""

        self.root = tk.Tk()
        self.root.title("American States Quiz - Census Vector Prototype")
        self.root.configure(bg=BACKGROUND)
        self.root.bind("<Escape>", lambda _event: self.root.destroy())
        self.root.bind("r", lambda _event: self.reset())
        self.root.bind("R", lambda _event: self.reset())

        self.prompt = tk.StringVar()
        self.status = tk.StringVar()

        top = tk.Frame(self.root, bg=BACKGROUND)
        top.pack(fill=tk.X)
        tk.Label(top, textvariable=self.prompt, font=("Segoe UI", 22, "bold"), fg=TEXT, bg=BACKGROUND).pack(
            side=tk.LEFT, padx=18, pady=(12, 8)
        )
        tk.Button(top, text="Reset", command=self.reset).pack(side=tk.RIGHT, padx=18, pady=(12, 8))

        self.canvas = tk.Canvas(self.root, width=CANVAS_W, height=CANVAS_H, bg=BACKGROUND, highlightthickness=0)
        self.canvas.pack()
        self.canvas.bind("<Button-1>", self.handle_click)

        bottom = tk.Frame(self.root, bg=BACKGROUND)
        bottom.pack(fill=tk.X)
        tk.Label(bottom, textvariable=self.status, font=("Segoe UI", 13), fg=STATUS, bg=BACKGROUND).pack(
            padx=18, pady=(0, 10)
        )

        self.draw()

    def target_name(self) -> str | None:
        if self.current_index >= len(QUIZ_ORDER):
            return None
        return QUIZ_ORDER[self.current_index]

    def draw(self) -> None:
        self.canvas.delete("all")
        for state in self.states:
            fill = MAP_FILL
            if state.name in self.completed:
                fill = COMPLETED_COLORS[QUIZ_ORDER.index(state.name) % len(COMPLETED_COLORS)]
            outline = TARGET_OUTLINE if state.name == self.target_name() else BORDER
            width = 2 if state.name == self.target_name() else 1
            state.canvas_ids.clear()
            for ring in state.screen_rings:
                item_id = self.canvas.create_polygon(
                    flatten_ring(ring),
                    fill=fill,
                    outline=outline,
                    width=width,
                    joinstyle=tk.ROUND,
                )
                state.canvas_ids.append(item_id)

        self.prompt.set("Done" if self.target_name() is None else f"Tap {self.target_name()}")
        self.status.set(self.last_status or "Census vector prototype")

    def reset(self) -> None:
        self.current_index = 0
        self.completed.clear()
        self.last_status = "Reset"
        self.draw()

    def state_at(self, point: Point) -> StateShape | None:
        for state in reversed(self.states):
            if point_in_state(point, state):
                return state
        return None

    def handle_click(self, event: tk.Event) -> None:
        clicked = self.state_at((event.x, event.y))
        if clicked is None:
            self.last_status = "Outside map"
            self.draw()
            return

        target = self.target_name()
        if target is None:
            self.last_status = "Done"
            self.draw()
            return

        if clicked.name == target:
            self.completed.add(clicked.name)
            self.current_index += 1
            self.last_status = "Done" if self.target_name() is None else "Correct"
        else:
            self.last_status = f"Try again: {clicked.name}"
        self.draw()

    def save_preview(self) -> bool:
        if Image is None or ImageDraw is None:
            return False

        image = Image.new("RGB", (CANVAS_W, CANVAS_H), BACKGROUND)
        draw = ImageDraw.Draw(image)
        for state in self.states:
            for ring in state.screen_rings:
                draw.polygon(ring, fill=MAP_FILL, outline=BORDER)
        target = self.state_by_name.get(QUIZ_ORDER[0])
        if target is not None:
            for ring in target.screen_rings:
                draw.line(ring + [ring[0]], fill=TARGET_OUTLINE, width=2)

        ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
        image.save(PREVIEW_PATH)
        return True

    def run(self) -> None:
        self.root.mainloop()


def smoke_test(states: list[StateShape]) -> bool:
    by_name = {state.name: state for state in states}
    ok = True
    for name in QUIZ_ORDER:
        state = by_name.get(name)
        if state is None:
            print(f"Missing quiz state: {name}")
            ok = False
            continue
        center = state_center(state)
        hit = next((candidate for candidate in states if point_in_state(center, candidate)), None)
        print(f"{name}: center hit {hit.name if hit else 'none'}")
        if hit is None:
            ok = False
    return ok


def main() -> None:
    shapefile_path = find_shapefile()
    if shapefile_path is None:
        print("Missing Census shapefile.")
        print("Run: python src/apps/geography/us_states_quiz/vector_pipeline/import_census_states.py")
        raise SystemExit(1)

    states, all_state_count = load_states(shapefile_path)
    project_states(states)

    if "--self-test" in sys.argv:
        print(f"Shapefile: {shapefile_path}")
        print(f"Records loaded: {all_state_count}")
        print(f"Contiguous states rendered: {len(states)}")
        raise SystemExit(0 if smoke_test(states) else 1)

    app = VectorQuizApp(states, shapefile_path, all_state_count)
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

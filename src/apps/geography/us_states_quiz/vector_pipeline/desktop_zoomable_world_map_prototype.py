from __future__ import annotations

from dataclasses import dataclass, field
from pathlib import Path
import sys
import tkinter as tk
import urllib.error
import urllib.request
import zipfile
from typing import Sequence

import shapefile

try:
    from PIL import Image, ImageDraw
except ImportError:
    Image = None
    ImageDraw = None


ROOT = Path(__file__).resolve().parents[5]
PIPELINE_DIR = Path(__file__).resolve().parent
DATA_DIR = PIPELINE_DIR / "data" / "world"
ARTIFACT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "artifacts"
PREVIEW_PATH = ARTIFACT_DIR / "desktop_zoomable_world_map_preview.png"

NATURAL_EARTH_URL = "https://naturalearth.s3.amazonaws.com/110m_cultural/ne_110m_admin_0_countries.zip"
ZIP_PATH = DATA_DIR / "ne_110m_admin_0_countries.zip"
EXTRACT_DIR = DATA_DIR / "ne_110m_admin_0_countries"
SHAPEFILE_NAME = "ne_110m_admin_0_countries.shp"

WINDOW_W = 1000
WINDOW_H = 650
TOP_H = 72
CANVAS_W = WINDOW_W
CANVAS_H = WINDOW_H - TOP_H
PAD_X = 26
PAD_Y = 24

OCEAN = "#27346D"
BACKGROUND = OCEAN
COUNTRY_PALETTE = [
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
LABEL_TEXT = "#f8fafc"
LABEL_SHADOW = "#101418"
OCEAN_LABEL = "#eaf6ff"
OCEAN_LABEL_SHADOW = "#31607d"

MIN_ZOOM = 1.0
MAX_ZOOM = 12.0
WHEEL_FACTOR = 1.18
KEY_FACTOR = 1.25
DRAG_CLICK_THRESHOLD = 4
BIT_CELL_SIZE = 5.0
COUNTRY_LABEL_ZOOM = 1.6
CAPITAL_LABEL_ZOOM = 2.2

Point = tuple[float, float]
Ring = list[Point]


@dataclass
class CountryShape:
    name: str
    admin: str
    source_rings: list[Ring]
    world_rings: list[Ring] = field(default_factory=list)
    bit_cells: list[tuple[float, float, float, float]] = field(default_factory=list)
    label_point: Point = (0.0, 0.0)


@dataclass
class CapitalMarker:
    country: str
    capital: str
    lon: float
    lat: float
    world_x: float = 0.0
    world_y: float = 0.0


@dataclass
class OceanLabel:
    name: str
    lon: float
    lat: float
    world_x: float = 0.0
    world_y: float = 0.0


CAPITALS: list[CapitalMarker] = [
    CapitalMarker("India", "New Delhi", 77.2090, 28.6139),
    CapitalMarker("China", "Beijing", 116.4074, 39.9042),
    CapitalMarker("United States of America", "Washington, D.C.", -77.0369, 38.9072),
    CapitalMarker("Indonesia", "Jakarta", 106.8456, -6.2088),
    CapitalMarker("Pakistan", "Islamabad", 73.0479, 33.6844),
    CapitalMarker("Brazil", "Brasilia", -47.8825, -15.7942),
    CapitalMarker("Nigeria", "Abuja", 7.3986, 9.0765),
    CapitalMarker("Bangladesh", "Dhaka", 90.4125, 23.8103),
    CapitalMarker("Russia", "Moscow", 37.6173, 55.7558),
    CapitalMarker("Mexico", "Mexico City", -99.1332, 19.4326),
    CapitalMarker("Japan", "Tokyo", 139.6917, 35.6895),
    CapitalMarker("Ethiopia", "Addis Ababa", 38.7578, 8.9806),
    CapitalMarker("Philippines", "Manila", 120.9842, 14.5995),
    CapitalMarker("Egypt", "Cairo", 31.2357, 30.0444),
    CapitalMarker("Vietnam", "Hanoi", 105.8342, 21.0278),
    CapitalMarker("Democratic Republic of the Congo", "Kinshasa", 15.2663, -4.4419),
    CapitalMarker("Turkey", "Ankara", 32.8597, 39.9334),
    CapitalMarker("Iran", "Tehran", 51.3890, 35.6892),
    CapitalMarker("Germany", "Berlin", 13.4050, 52.5200),
    CapitalMarker("Thailand", "Bangkok", 100.5018, 13.7563),
    CapitalMarker("United Kingdom", "London", -0.1276, 51.5072),
    CapitalMarker("France", "Paris", 2.3522, 48.8566),
    CapitalMarker("Italy", "Rome", 12.4964, 41.9028),
    CapitalMarker("South Africa", "Pretoria", 28.2293, -25.7479),
    CapitalMarker("Tanzania", "Dodoma", 35.7516, -6.1630),
    CapitalMarker("Myanmar", "Naypyidaw", 96.0785, 19.7633),
    CapitalMarker("Kenya", "Nairobi", 36.8219, -1.2921),
    CapitalMarker("South Korea", "Seoul", 126.9780, 37.5665),
    CapitalMarker("Colombia", "Bogota", -74.0721, 4.7110),
    CapitalMarker("Spain", "Madrid", -3.7038, 40.4168),
]

OCEANS: list[OceanLabel] = [
    OceanLabel("Pacific Ocean", -150.0, 5.0),
    OceanLabel("Atlantic Ocean", -35.0, 10.0),
    OceanLabel("Indian Ocean", 75.0, -25.0),
    OceanLabel("Arctic Ocean", 20.0, 78.0),
    OceanLabel("Southern Ocean", 20.0, -58.0),
]


def ensure_shapefile() -> Path | None:
    expected = EXTRACT_DIR / SHAPEFILE_NAME
    if expected.exists():
        return expected

    candidates = sorted(DATA_DIR.rglob("*.shp"))
    if candidates:
        return candidates[0]

    DATA_DIR.mkdir(parents=True, exist_ok=True)
    try:
        print(f"Downloading Natural Earth countries: {NATURAL_EARTH_URL}")
        urllib.request.urlretrieve(NATURAL_EARTH_URL, ZIP_PATH)
        EXTRACT_DIR.mkdir(parents=True, exist_ok=True)
        with zipfile.ZipFile(ZIP_PATH) as archive:
            archive.extractall(EXTRACT_DIR)
    except (urllib.error.URLError, OSError, zipfile.BadZipFile) as exc:
        print(f"Could not download/extract Natural Earth countries: {exc}")
        print(f"Place the Natural Earth Admin 0 countries zip here: {ZIP_PATH}")
        print("Suggested file: ne_110m_admin_0_countries.zip")
        return None

    return expected if expected.exists() else None


def shape_to_rings(shape: shapefile.Shape) -> list[Ring]:
    points = shape.points
    parts = list(shape.parts) + [len(points)]
    rings: list[Ring] = []
    for index in range(len(parts) - 1):
        ring = [(float(x), float(y)) for x, y in points[parts[index] : parts[index + 1]]]
        if len(ring) >= 3:
            rings.append(ring)
    return rings


def load_countries(path: Path) -> tuple[list[CountryShape], int]:
    reader = shapefile.Reader(str(path), encoding="latin1")
    fields = [field[0] for field in reader.fields[1:]]
    countries: list[CountryShape] = []
    all_count = 0

    for shape_record in reader.iterShapeRecords():
        all_count += 1
        record = dict(zip(fields, shape_record.record))
        name = str(record.get("NAME_LONG") or record.get("ADMIN") or record.get("NAME") or "")
        admin = str(record.get("ADMIN") or name)
        if name == "Antarctica" or admin == "Antarctica":
            continue
        rings = shape_to_rings(shape_record.shape)
        if not rings:
            continue
        countries.append(CountryShape(name=name, admin=admin, source_rings=rings))

    project_world(countries)
    return countries, all_count


def source_bounds(countries: Sequence[CountryShape]) -> tuple[float, float, float, float]:
    xs = [x for country in countries for ring in country.source_rings for x, _ in ring]
    ys = [y for country in countries for ring in country.source_rings for _, y in ring]
    return min(xs), min(ys), max(xs), max(ys)


def project_world(countries: Sequence[CountryShape]) -> None:
    min_x, min_y, max_x, max_y = source_bounds(countries)
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

    for country in countries:
        country.world_rings = [[project(point) for point in ring] for ring in country.source_rings]
        country.bit_cells = build_country_bit_cells(country)
        country.label_point = find_country_label_point(country)

    for marker in CAPITALS:
        marker.world_x, marker.world_y = project((marker.lon, marker.lat))
    for label in OCEANS:
        label.world_x, label.world_y = project((label.lon, label.lat))


def flatten_ring(ring: Ring) -> list[float]:
    coords: list[float] = []
    for x, y in ring:
        coords.extend((x, y))
    return coords


def muted_country_color(name: str) -> str:
    palette_index = sum((index + 1) * ord(char) for index, char in enumerate(name)) % len(COUNTRY_PALETTE)
    return COUNTRY_PALETTE[palette_index]


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


def point_in_country(point: Point, country: CountryShape) -> bool:
    return any(point_in_ring(point, ring) for ring in country.world_rings)


def country_world_bounds(country: CountryShape) -> tuple[float, float, float, float]:
    xs = [x for ring in country.world_rings for x, _ in ring]
    ys = [y for ring in country.world_rings for _, y in ring]
    return min(xs), min(ys), max(xs), max(ys)


def find_country_label_point(country: CountryShape) -> Point:
    min_x, min_y, max_x, max_y = country_world_bounds(country)
    center = ((min_x + max_x) / 2, (min_y + max_y) / 2)
    if point_in_country(center, country):
        return center
    points = [point for ring in country.world_rings for point in ring]
    return min(
        points,
        key=lambda point: (point[0] - center[0]) * (point[0] - center[0])
        + (point[1] - center[1]) * (point[1] - center[1]),
    )


def build_country_bit_cells(country: CountryShape) -> list[tuple[float, float, float, float]]:
    min_x, min_y, max_x, max_y = country_world_bounds(country)
    start_x = int(min_x // BIT_CELL_SIZE) * BIT_CELL_SIZE
    start_y = int(min_y // BIT_CELL_SIZE) * BIT_CELL_SIZE
    cells: list[tuple[float, float, float, float]] = []
    y = start_y
    while y <= max_y:
        x = start_x
        while x <= max_x:
            center = (x + BIT_CELL_SIZE / 2, y + BIT_CELL_SIZE / 2)
            if point_in_country(center, country):
                cells.append((x, y, x + BIT_CELL_SIZE, y + BIT_CELL_SIZE))
            x += BIT_CELL_SIZE
        y += BIT_CELL_SIZE
    return cells


def country_name_matches(country: CountryShape, target: str) -> bool:
    aliases = {
        "United States of America": {"United States", "United States of America"},
        "Democratic Republic of the Congo": {"Dem. Rep. Congo", "Democratic Republic of the Congo", "DR Congo"},
        "Russia": {"Russia", "Russian Federation"},
        "South Korea": {"South Korea", "Republic of Korea", "Korea, Republic of"},
        "Tanzania": {"Tanzania", "United Republic of Tanzania"},
        "Myanmar": {"Myanmar", "Burma"},
    }
    target_names = aliases.get(target, {target})
    return country.name in target_names or country.admin in target_names


class ZoomableWorldMapApp:
    def __init__(self, countries: list[CountryShape], shapefile_path: Path, all_country_count: int) -> None:
        self.countries = countries
        self.shapefile_path = shapefile_path
        self.all_country_count = all_country_count
        self.focus_countries = [country for country in countries if any(country_name_matches(country, c.country) for c in CAPITALS)]
        self.camera_center_x = CANVAS_W / 2
        self.camera_center_y = CANVAS_H / 2
        self.zoom = 1.0
        self.clicked_country = ""
        self.click_message = "Click a country"
        self.show_grid = True
        self.show_capitals = True
        self.show_labels = True
        self.drag_last: Point | None = None
        self.drag_total = 0.0

        self.root = tk.Tk()
        self.root.title("BitSlate Zoomable World Map Prototype")
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
        tk.Label(top, textvariable=self.prompt, font=("Segoe UI", 20, "bold"), fg=TEXT, bg=BACKGROUND).pack(
            side=tk.LEFT, padx=18, pady=(10, 0)
        )
        tk.Label(top, textvariable=self.status, font=("Segoe UI", 11), fg=STATUS, bg=BACKGROUND).pack(
            side=tk.RIGHT, padx=18, pady=(15, 0)
        )

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
        return (wx - self.camera_center_x) * self.zoom + CANVAS_W / 2, (wy - self.camera_center_y) * self.zoom + CANVAS_H / 2

    def screen_to_world(self, sx: float, sy: float) -> Point:
        return (sx - CANVAS_W / 2) / self.zoom + self.camera_center_x, (sy - CANVAS_H / 2) / self.zoom + self.camera_center_y

    def screen_ring(self, ring: Ring) -> Ring:
        return [self.world_to_screen(x, y) for x, y in ring]

    def draw(self) -> None:
        self.canvas.delete("all")
        for country in self.countries:
            outline = CLICKED_OUTLINE if country.name == self.clicked_country else BORDER
            width = 2 if country.name == self.clicked_country else 1
            for ring in country.world_rings:
                self.canvas.create_polygon(
                    flatten_ring(self.screen_ring(ring)),
                    fill=muted_country_color(country.name),
                    outline=outline,
                    width=width,
                    joinstyle=tk.ROUND,
                )

        if self.show_grid:
            for country in self.countries:
                for x1, y1, x2, y2 in country.bit_cells:
                    sx1, sy1 = self.world_to_screen(x1, y1)
                    sx2, sy2 = self.world_to_screen(x2, y2)
                    if sx2 < -2 or sy2 < -2 or sx1 > CANVAS_W + 2 or sy1 > CANVAS_H + 2:
                        continue
                    self.canvas.create_rectangle(sx1, sy1, sx2, sy2, outline=BIT_GRID, width=1)

        for country in self.countries:
            outline = CLICKED_OUTLINE if country.name == self.clicked_country else BORDER
            width = 2 if country.name == self.clicked_country else 1
            for ring in country.world_rings:
                self.canvas.create_line(flatten_ring(self.screen_ring(ring + [ring[0]])), fill=outline, width=width)

        if self.show_capitals:
            self.draw_capitals()
        if self.show_labels:
            self.draw_labels()

        self.prompt.set("Zoomable World Map")
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

    def draw_text(self, sx: float, sy: float, text: str, fill: str, shadow: str, font: tuple[str, int, str] | tuple[str, int], anchor: str = tk.CENTER) -> None:
        self.canvas.create_text(sx + 1, sy + 1, text=text, fill=shadow, font=font, anchor=anchor)
        self.canvas.create_text(sx, sy, text=text, fill=fill, font=font, anchor=anchor)

    def capital_radius(self) -> float:
        return max(3.0, min(7.0, 3.0 * (self.zoom ** 0.35)))

    def draw_capitals(self) -> None:
        radius = self.capital_radius()
        for marker in CAPITALS:
            sx, sy = self.world_to_screen(marker.world_x, marker.world_y)
            if not self.point_on_screen(sx, sy):
                continue
            self.canvas.create_oval(sx - radius, sy - radius, sx + radius, sy + radius, fill=CAPITAL_FILL, outline=CAPITAL_OUTLINE, width=2)

    def draw_labels(self) -> None:
        for ocean in OCEANS:
            sx, sy = self.world_to_screen(ocean.world_x, ocean.world_y)
            if self.point_on_screen(sx, sy, 120):
                self.draw_text(sx, sy, ocean.name, OCEAN_LABEL, OCEAN_LABEL_SHADOW, ("Segoe UI", 12, "bold"))

        if self.zoom >= COUNTRY_LABEL_ZOOM:
            for country in self.focus_countries:
                sx, sy = self.world_to_screen(*country.label_point)
                if self.point_on_screen(sx, sy, 80):
                    self.draw_text(sx, sy, country.name, LABEL_TEXT, LABEL_SHADOW, ("Segoe UI", 8, "bold"))

        if self.zoom >= CAPITAL_LABEL_ZOOM and self.show_capitals:
            radius = self.capital_radius()
            for marker in CAPITALS:
                sx, sy = self.world_to_screen(marker.world_x, marker.world_y)
                if self.point_on_screen(sx, sy, 80):
                    self.draw_text(sx, sy - radius - 9, marker.capital, LABEL_TEXT, LABEL_SHADOW, ("Segoe UI", 8), anchor=tk.S)

    def point_on_screen(self, sx: float, sy: float, margin: int = 32) -> bool:
        return -margin <= sx <= CANVAS_W + margin and -margin <= sy <= CANVAS_H + margin

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

    def country_at_world(self, point: Point) -> CountryShape | None:
        for country in reversed(self.countries):
            if point_in_country(point, country):
                return country
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
        self.drag_last = (float(event.x), float(event.y))
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
                self.clicked_country = capital.country
                self.click_message = f"Capital: {capital.capital}, {capital.country}"
                print(self.click_message)
                self.draw()
                self.drag_last = None
                return
            clicked = self.country_at_world(self.screen_to_world(float(event.x), float(event.y)))
            self.clicked_country = clicked.name if clicked is not None else ""
            self.click_message = f"Clicked: {clicked.name}" if clicked is not None else "Ocean / outside country"
            print(self.click_message)
            self.draw()
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
        return save_preview(self.countries)

    def run(self) -> None:
        self.root.mainloop()


def save_preview(countries: list[CountryShape]) -> bool:
    if Image is None or ImageDraw is None:
        return False
    image = Image.new("RGB", (CANVAS_W, CANVAS_H), BACKGROUND)
    draw = ImageDraw.Draw(image)
    for country in countries:
        for ring in country.world_rings:
            draw.polygon(ring, fill=muted_country_color(country.name), outline=BORDER)
    for country in countries:
        for x1, y1, x2, y2 in country.bit_cells:
            draw.rectangle((x1, y1, x2, y2), outline=BIT_GRID)
    for country in countries:
        for ring in country.world_rings:
            draw.line(ring + [ring[0]], fill=BORDER, width=1)
    for marker in CAPITALS:
        radius = 3
        draw.ellipse((marker.world_x - radius, marker.world_y - radius, marker.world_x + radius, marker.world_y + radius), fill=CAPITAL_FILL, outline=CAPITAL_OUTLINE, width=2)
    for ocean in OCEANS:
        draw.text((ocean.world_x + 1, ocean.world_y + 1), ocean.name, fill=OCEAN_LABEL_SHADOW)
        draw.text((ocean.world_x, ocean.world_y), ocean.name, fill=OCEAN_LABEL)
    ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    image.save(PREVIEW_PATH)
    return True


def smoke_test(countries: list[CountryShape]) -> bool:
    ok = True
    focus_hits = 0
    for marker in CAPITALS:
        country = next((candidate for candidate in countries if country_name_matches(candidate, marker.country)), None)
        if country is None:
            print(f"Missing focus country: {marker.country}")
            ok = False
            continue
        focus_hits += 1
        if not point_in_country((marker.world_x, marker.world_y), country):
            print(f"Capital marker outside expected country: {marker.capital}, {marker.country}")
            ok = False
    print(f"Focus countries/capitals checked: {focus_hits}")
    print(f"Ocean labels projected: {len(OCEANS)}")
    return ok


def main() -> None:
    shapefile_path = ensure_shapefile()
    if shapefile_path is None:
        raise SystemExit(1)

    countries, all_country_count = load_countries(shapefile_path)
    if "--self-test" in sys.argv:
        saved = save_preview(countries)
        print(f"Shapefile: {shapefile_path}")
        print(f"Records loaded: {all_country_count}")
        print(f"Countries rendered: {len(countries)}")
        print(f"Preview artifact: {PREVIEW_PATH if saved else 'skipped; Pillow unavailable'}")
        raise SystemExit(0 if smoke_test(countries) else 1)

    app = ZoomableWorldMapApp(countries, shapefile_path, all_country_count)
    saved = app.save_preview()
    print(f"Shapefile: {shapefile_path}")
    print(f"Records loaded: {all_country_count}")
    print(f"Countries rendered: {len(countries)}")
    print(f"Included capitals: {', '.join(marker.capital for marker in CAPITALS)}")
    print(f"Preview artifact: {PREVIEW_PATH if saved else 'skipped; Pillow unavailable'}")
    app.run()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)

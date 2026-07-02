from __future__ import annotations

from pathlib import Path
import sys
from typing import Iterable

from PIL import Image, ImageDraw


ROOT = Path(__file__).resolve().parents[5]
PIPELINE_DIR = Path(__file__).resolve().parent
DATA_DIR = PIPELINE_DIR / "data"
ARTIFACT_DIR = ROOT / "src" / "apps" / "geography" / "us_states_quiz" / "artifacts"
OUTPUT = ARTIFACT_DIR / "census_vector_border_preview.png"

MAP_W = 390
MAP_H = 250
BACKGROUND = (241, 244, 247)
BASE_MAP = (47, 52, 55)
BORDER = (247, 247, 247)

EXCLUDED = {"02", "15", "60", "66", "69", "72", "78"}


def find_shapefile() -> Path | None:
    candidates = sorted(DATA_DIR.rglob("*.shp"))
    return candidates[0] if candidates else None


def try_geopandas(path: Path):
    try:
        import geopandas as gpd
    except ImportError:
        return None

    states = gpd.read_file(path)
    if "STATEFP" in states.columns:
        states = states[~states["STATEFP"].isin(EXCLUDED)]
    states = states.to_crs("EPSG:5070")
    return list(states.geometry)


def try_pyshp(path: Path):
    try:
        import shapefile
    except ImportError:
        return None

    reader = shapefile.Reader(str(path))
    fields = [field[0] for field in reader.fields[1:]]
    statefp_index = fields.index("STATEFP") if "STATEFP" in fields else None
    geometries = []

    for record_shape in reader.iterShapeRecords():
        if statefp_index is not None and record_shape.record[statefp_index] in EXCLUDED:
            continue
        geometries.append(record_shape.shape.__geo_interface__)

    return geometries


def bounds_for_geometries(geometries: Iterable) -> tuple[float, float, float, float]:
    min_x = min_y = float("inf")
    max_x = max_y = float("-inf")

    def visit_coords(coords) -> None:
        nonlocal min_x, min_y, max_x, max_y
        if isinstance(coords[0], (float, int)):
            x, y = coords[:2]
            min_x = min(min_x, x)
            min_y = min(min_y, y)
            max_x = max(max_x, x)
            max_y = max(max_y, y)
            return
        for item in coords:
            visit_coords(item)

    for geometry in geometries:
        if hasattr(geometry, "__geo_interface__"):
            geometry = geometry.__geo_interface__
        visit_coords(geometry["coordinates"])

    return min_x, min_y, max_x, max_y


def draw_geometry(draw: ImageDraw.ImageDraw, geometry, bounds: tuple[float, float, float, float]) -> None:
    if hasattr(geometry, "__geo_interface__"):
        geometry = geometry.__geo_interface__

    min_x, min_y, max_x, max_y = bounds
    scale = min((MAP_W - 8) / (max_x - min_x), (MAP_H - 8) / (max_y - min_y))
    offset_x = (MAP_W - (max_x - min_x) * scale) / 2
    offset_y = (MAP_H - (max_y - min_y) * scale) / 2

    def project(point):
        x, y = point[:2]
        return (
            offset_x + (x - min_x) * scale,
            MAP_H - (offset_y + (y - min_y) * scale),
        )

    def project_ring(ring):
        return [project(point) for point in ring]

    def draw_polygon(rings) -> None:
        if not rings:
            return
        outer = project_ring(rings[0])
        if len(outer) >= 3:
            draw.polygon(outer, fill=BASE_MAP)
            draw.line(outer, fill=BORDER, width=1)
        for hole in rings[1:]:
            points = project_ring(hole)
            if len(points) >= 3:
                draw.polygon(points, fill=BACKGROUND)
                draw.line(points, fill=BORDER, width=1)

    def draw_ring(ring) -> None:
        points = project_ring(ring)
        if len(points) >= 2:
            draw.line(points, fill=BORDER, width=1)

    geometry_type = geometry["type"]
    coordinates = geometry["coordinates"]
    if geometry_type == "Polygon":
        draw_polygon(coordinates)
    elif geometry_type == "MultiPolygon":
        for polygon in coordinates:
            draw_polygon(polygon)


def main() -> None:
    shapefile_path = find_shapefile()
    if shapefile_path is None:
        print(f"No Census shapefile found. Run import_census_states.py or place the zip/extracted shapefile under {DATA_DIR}")
        return

    geometries = try_geopandas(shapefile_path)
    source = "geopandas"
    if geometries is None:
        geometries = try_pyshp(shapefile_path)
        source = "pyshp"
    if geometries is None:
        print("Missing optional dependency: install geopandas/shapely or pyshp to render the vector preview.")
        return

    bounds = bounds_for_geometries(geometries)
    image = Image.new("RGB", (MAP_W, MAP_H), BACKGROUND)
    draw = ImageDraw.Draw(image)
    for geometry in geometries:
        draw_geometry(draw, geometry, bounds)

    ARTIFACT_DIR.mkdir(parents=True, exist_ok=True)
    image.save(OUTPUT)
    print(f"Saved {OUTPUT} using {source}")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(130)

from __future__ import annotations

from collections import deque
from pathlib import Path
from typing import Dict, Iterable, List, Optional, Sequence, Set, Tuple

from PIL import Image, ImageFilter

import generated_us_map as generated
import map_transform


ROOT = Path(__file__).resolve().parents[5]
SOURCE_IMAGE = ROOT / "src" / "apps" / "assets" / "images" / "geography" / "usa-map-full.png"
OUTPUT_FILE = Path(__file__).with_name("generated_state_masks.py")

GRID_POINT = Tuple[int, int]
RGB = Tuple[int, int, int]
CleanupConfig = Tuple[int, int]

SEED_CELLS: Dict[str, GRID_POINT] = {
    "California": (9, 54),
    "Texas": (70, 83),
    "Washington": (15, 6),
    "South Dakota": (67, 30),
    "Alaska": (16, 88),
    "Missouri": (95, 50),
}

INTERNAL_SCALE = 2
QUANTIZED_COLOR_COUNT = 24
LOCAL_EDGE_THRESHOLD = 20.0
SEED_COLOR_THRESHOLD = 20.0
DOWNSAMPLE_MIN_HITS = 2
STATE_DOWNSAMPLE_MIN_HITS: Dict[str, int] = {
    "Texas": 3,
    "Alaska": 1,
}
NEIGHBORS_4: Tuple[GRID_POINT, ...] = ((1, 0), (-1, 0), (0, 1), (0, -1))
NEIGHBORS_8: Tuple[GRID_POINT, ...] = tuple(
    (dx, dy)
    for dy in (-1, 0, 1)
    for dx in (-1, 0, 1)
    if dx != 0 or dy != 0
)
CLEANUP_CONFIG: Dict[str, CleanupConfig] = {
    "Texas": (5, 2),
    "Washington": (6, 2),
    "Alaska": (4, 2),
}


def crop_source_image() -> Image.Image:
    image = Image.open(SOURCE_IMAGE).convert("RGBA")
    return image.crop(map_transform.crop_box(map_transform.COLOR_SOURCE_BOUNDS))


def color_distance(a: RGB, b: RGB) -> float:
    return ((a[0] - b[0]) ** 2 + (a[1] - b[1]) ** 2 + (a[2] - b[2]) ** 2) ** 0.5


def build_boundary_source(crop: Image.Image) -> Image.Image:
    smoothed = crop.convert("RGB").filter(ImageFilter.MedianFilter(size=5))
    smoothed = smoothed.filter(ImageFilter.SMOOTH_MORE)
    return smoothed.quantize(colors=QUANTIZED_COLOR_COUNT).convert("RGB")


def build_high_res_color_grid(source: Image.Image) -> List[List[Optional[RGB]]]:
    width = generated.GRID_WIDTH * INTERNAL_SCALE
    height = generated.GRID_HEIGHT * INTERNAL_SCALE
    grid: List[List[Optional[RGB]]] = []

    for y in range(height):
        row: List[Optional[RGB]] = []
        grid_y = y // INTERNAL_SCALE
        source_y = min(source.height - 1, int((y + 0.5) * source.height / height))

        for x in range(width):
            grid_x = x // INTERNAL_SCALE
            if generated.CELL_MASK[grid_y][grid_x] != "1":
                row.append(None)
                continue

            source_x = min(source.width - 1, int((x + 0.5) * source.width / width))
            row.append(source.getpixel((source_x, source_y)))
        grid.append(row)

    return grid


def can_cross_boundary(
    color_grid: Sequence[Sequence[Optional[RGB]]],
    current: GRID_POINT,
    neighbor: GRID_POINT,
    seed_color: RGB,
) -> bool:
    x, y = current
    nx, ny = neighbor
    current_color = color_grid[y][x]
    neighbor_color = color_grid[ny][nx]
    if current_color is None or neighbor_color is None:
        return False

    if color_distance(current_color, neighbor_color) > LOCAL_EDGE_THRESHOLD:
        return False
    if color_distance(neighbor_color, seed_color) > SEED_COLOR_THRESHOLD:
        return False

    return True


def flood_fill_high_res_region(
    color_grid: Sequence[Sequence[Optional[RGB]]],
    seed: GRID_POINT,
) -> Set[GRID_POINT]:
    width = generated.GRID_WIDTH * INTERNAL_SCALE
    height = generated.GRID_HEIGHT * INTERNAL_SCALE
    seed_x = seed[0] * INTERNAL_SCALE + INTERNAL_SCALE // 2
    seed_y = seed[1] * INTERNAL_SCALE + INTERNAL_SCALE // 2

    if not (0 <= seed_x < width and 0 <= seed_y < height):
        raise ValueError(f"Seed {seed} is outside the generated grid")

    seed_color = color_grid[seed_y][seed_x]
    if seed_color is None:
        raise ValueError(f"Seed {seed} does not land on a colored map cell")

    visited: Set[GRID_POINT] = set()
    region: Set[GRID_POINT] = set()
    queue: deque[GRID_POINT] = deque([(seed_x, seed_y)])

    while queue:
        x, y = queue.popleft()
        if (x, y) in visited:
            continue
        visited.add((x, y))

        if color_grid[y][x] is None:
            continue

        region.add((x, y))
        for dx, dy in NEIGHBORS_4:
            nx = x + dx
            ny = y + dy
            if not (0 <= nx < width and 0 <= ny < height):
                continue
            if (nx, ny) in visited:
                continue
            if can_cross_boundary(color_grid, (x, y), (nx, ny), seed_color):
                queue.append((nx, ny))

    return region


def downsample_region(region: Set[GRID_POINT], min_hits: int = DOWNSAMPLE_MIN_HITS) -> Set[GRID_POINT]:
    mask: Set[GRID_POINT] = set()
    for grid_y in range(generated.GRID_HEIGHT):
        for grid_x in range(generated.GRID_WIDTH):
            hits = 0
            for sub_y in range(INTERNAL_SCALE):
                for sub_x in range(INTERNAL_SCALE):
                    point = (grid_x * INTERNAL_SCALE + sub_x, grid_y * INTERNAL_SCALE + sub_y)
                    if point in region:
                        hits += 1
            if hits >= min_hits:
                mask.add((grid_x, grid_y))
    return mask


def mask_bounds(mask: Set[GRID_POINT]) -> Tuple[int, int, int, int]:
    xs = [point[0] for point in mask]
    ys = [point[1] for point in mask]
    return min(xs), min(ys), max(xs), max(ys)


def fill_small_holes(mask: Set[GRID_POINT], min_neighbors: int) -> Set[GRID_POINT]:
    if not mask:
        return set()

    filled = set(mask)
    min_x, min_y, max_x, max_y = mask_bounds(filled)
    for y in range(max(0, min_y - 1), min(generated.GRID_HEIGHT, max_y + 2)):
        for x in range(max(0, min_x - 1), min(generated.GRID_WIDTH, max_x + 2)):
            if (x, y) in filled or generated.CELL_MASK[y][x] != "1":
                continue
            neighbor_count = sum((x + dx, y + dy) in filled for dx, dy in NEIGHBORS_8)
            if neighbor_count >= min_neighbors:
                filled.add((x, y))
    return filled


def prune_isolated_cells(mask: Set[GRID_POINT], min_neighbors: int) -> Set[GRID_POINT]:
    pruned = set(mask)
    for point in mask:
        x, y = point
        neighbor_count = sum((x + dx, y + dy) in mask for dx, dy in NEIGHBORS_8)
        if neighbor_count < min_neighbors:
            pruned.discard(point)
    return pruned


def connected_components(mask: Set[GRID_POINT]) -> List[Set[GRID_POINT]]:
    remaining = set(mask)
    components: List[Set[GRID_POINT]] = []

    while remaining:
        seed = next(iter(remaining))
        remaining.remove(seed)
        component: Set[GRID_POINT] = set()
        queue: deque[GRID_POINT] = deque([seed])

        while queue:
            point = queue.popleft()
            component.add(point)
            x, y = point
            for dx, dy in NEIGHBORS_4:
                neighbor = (x + dx, y + dy)
                if neighbor in remaining:
                    remaining.remove(neighbor)
                    queue.append(neighbor)

        components.append(component)

    return components


def keep_largest_component(mask: Set[GRID_POINT]) -> Set[GRID_POINT]:
    components = connected_components(mask)
    if not components:
        return set()
    return set(max(components, key=len))


def cleanup_state_mask(state_name: str, mask: Set[GRID_POINT]) -> Set[GRID_POINT]:
    config = CLEANUP_CONFIG.get(state_name)
    if config is None:
        return mask

    fill_neighbor_count, prune_neighbor_count = config
    cleaned = fill_small_holes(mask, fill_neighbor_count)
    cleaned = prune_isolated_cells(cleaned, prune_neighbor_count)
    return keep_largest_component(cleaned)


def build_state_masks() -> Dict[str, Set[GRID_POINT]]:
    crop = crop_source_image()
    boundary_source = build_boundary_source(crop)
    color_grid = build_high_res_color_grid(boundary_source)

    state_masks: Dict[str, Set[GRID_POINT]] = {}
    for state_name, seed in SEED_CELLS.items():
        high_res_region = flood_fill_high_res_region(color_grid, seed)
        raw_mask = downsample_region(high_res_region, STATE_DOWNSAMPLE_MIN_HITS.get(state_name, DOWNSAMPLE_MIN_HITS))
        state_masks[state_name] = cleanup_state_mask(state_name, raw_mask)
    return state_masks


def serialize_masks(state_masks: Dict[str, Iterable[GRID_POINT]]) -> str:
    lines = [
        "# Auto-generated by generate_state_masks_from_color.py",
        "from __future__ import annotations",
        "",
        "from typing import Dict, List, Optional, Sequence, Set, Tuple",
        "",
        "GridPoint = Tuple[int, int]",
        "",
        "STATE_NAMES: Tuple[str, ...] = (",
    ]
    for state_name in SEED_CELLS:
        lines.append(f"    {state_name!r},")
    lines.extend(
        [
            ")",
            "",
            "SEED_CELLS: Dict[str, GridPoint] = {",
        ]
    )
    for state_name, seed in SEED_CELLS.items():
        lines.append(f"    {state_name!r}: {seed!r},")
    lines.extend(
        [
            "}",
            "",
            "STATE_MASKS: Dict[str, List[GridPoint]] = {",
        ]
    )
    for state_name in SEED_CELLS:
        points = sorted(state_masks[state_name], key=lambda pt: (pt[1], pt[0]))
        lines.append(f"    {state_name!r}: {points!r},")
    lines.extend(
        [
            "}",
            "",
            "",
            "def get_mask_for_state(name: str) -> List[GridPoint]:",
            "    return STATE_MASKS.get(name, [])",
            "",
            "",
            "def get_state_at_cell(x: int, y: int) -> Optional[str]:",
            "    for state_name, mask in STATE_MASKS.items():",
            "        if (x, y) in mask:",
            "            return state_name",
            "    return None",
            "",
        ]
    )
    return "\n".join(lines)


def main() -> None:
    state_masks = build_state_masks()
    OUTPUT_FILE.write_text(serialize_masks(state_masks), encoding="utf-8")

    seen: Dict[GRID_POINT, str] = {}
    overlap_cells: Set[GRID_POINT] = set()
    for state_name, mask in state_masks.items():
        for point in mask:
            previous = seen.get(point)
            if previous is not None and previous != state_name:
                overlap_cells.add(point)
            seen[point] = state_name

    print(f"Generated state masks -> {OUTPUT_FILE}")
    print(f"Internal extraction scale: {INTERNAL_SCALE}x")
    print(f"Quantized colors: {QUANTIZED_COLOR_COUNT}")
    print(f"Local edge threshold: {LOCAL_EDGE_THRESHOLD}")
    print(f"Seed color threshold: {SEED_COLOR_THRESHOLD}")
    for state_name, seed in SEED_CELLS.items():
        min_hits = STATE_DOWNSAMPLE_MIN_HITS.get(state_name, DOWNSAMPLE_MIN_HITS)
        cleanup = CLEANUP_CONFIG.get(state_name)
        print(f"{state_name}: seed={seed} min_hits={min_hits} cleanup={cleanup} cells={len(state_masks[state_name])}")
    print(f"Overlap cells across masks: {len(overlap_cells)}")


if __name__ == "__main__":
    main()

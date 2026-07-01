from __future__ import annotations

from typing import Dict, List, Optional, Sequence, Tuple

Point = Tuple[int, int]
Polygon = List[Point]

TRACE_STATES: Tuple[str, ...] = (
    "California",
    "Texas",
    "Washington",
    "Oregon",
    "Nevada",
    "Arizona",
    "Colorado",
    "New Mexico",
    "Utah",
    "Idaho",
    "Montana",
    "Wyoming",
    "Florida",
    "Georgia",
    "New York",
    "Pennsylvania",
    "Michigan",
    "Illinois",
    "Ohio",
    "North Carolina",
)

STATE_POLYGONS: Dict[str, Polygon] = {
    'California': [(0, 27), (13, 30), (11, 42), (22, 57), (22, 73), (18, 72), (14, 70), (12, 67), (10, 64), (6, 62), (5, 60), (1, 53), (0, 48), (0, 27)],
    'Texas': [(61, 59), (72, 60), (72, 71), (89, 72), (91, 90), (81, 97), (81, 104), (76, 104), (64, 90), (60, 92), (54, 87), (48, 80), (48, 78), (59, 79), (61, 59)],
    'Washington': [(5, 11), (23, 16), (28, 4), (20, 1), (13, 0), (12, 3), (9, 1), (6, 0), (4, 10)],
    'Oregon': [],
    'Nevada': [],
    'Arizona': [],
    'Colorado': [],
    'New Mexico': [],
    'Utah': [],
    'Idaho': [],
    'Montana': [],
    'Wyoming': [],
    'Florida': [],
    'Georgia': [],
    'New York': [],
    'Pennsylvania': [],
    'Michigan': [],
    'Illinois': [],
    'Ohio': [],
    'North Carolina': [],
}


def point_in_polygon(x: float, y: float, polygon: Sequence[Point]) -> bool:
    if len(polygon) < 3:
        return False

    inside = False
    j = len(polygon) - 1
    for i in range(len(polygon)):
        xi, yi = polygon[i]
        xj, yj = polygon[j]
        intersects = ((yi > y) != (yj > y)) and (
            x < (xj - xi) * (y - yi) / ((yj - yi) or 1e-9) + xi
        )
        if intersects:
            inside = not inside
        j = i
    return inside


def get_state_at_grid_cell(grid_x: int, grid_y: int) -> Optional[str]:
    for state_name, polygon in STATE_POLYGONS.items():
        if point_in_polygon(grid_x + 0.5, grid_y + 0.5, polygon):
            return state_name
    return None

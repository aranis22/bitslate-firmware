from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))

import generated_us_map as generated


def main() -> None:
    assert generated.GRID_WIDTH > 0
    assert generated.GRID_HEIGHT > 0
    assert generated.MAP_CELL_COUNT > 1000
    assert generated.MAP_CELL_COUNT < generated.GRID_WIDTH * generated.GRID_HEIGHT
    assert generated.BOUNDARY_CELL_COUNT > 0
    assert len(generated.CELL_MASK) == generated.GRID_HEIGHT
    assert len(generated.BOUNDARY_MASK) == generated.GRID_HEIGHT
    assert all(len(row) == generated.GRID_WIDTH for row in generated.CELL_MASK)
    assert all(len(row) == generated.GRID_WIDTH for row in generated.BOUNDARY_MASK)
    print("Generated US map tests passed.")


if __name__ == "__main__":
    main()

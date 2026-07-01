from __future__ import annotations

from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parent))

import generated_state_masks as masks


EXPECTED_STATES = {"California", "Texas", "Washington", "South Dakota", "Alaska", "Missouri"}


def main() -> None:
    assert set(masks.STATE_NAMES) == EXPECTED_STATES

    for state_name in masks.STATE_NAMES:
        assert len(masks.get_mask_for_state(state_name)) > 0, f"{state_name} mask is empty"

    all_cells = {}
    overlap_count = 0
    for state_name in masks.STATE_NAMES:
        for cell in masks.get_mask_for_state(state_name):
            if cell in all_cells and all_cells[cell] != state_name:
                overlap_count += 1
            all_cells[cell] = state_name
    assert overlap_count == 0, f"Found overlapping mask cells: {overlap_count}"

    assert masks.get_state_at_cell(9, 54) == "California"
    assert masks.get_state_at_cell(70, 83) == "Texas"
    assert masks.get_state_at_cell(15, 6) == "Washington"
    assert masks.get_state_at_cell(67, 30) == "South Dakota"
    assert masks.get_state_at_cell(16, 88) == "Alaska"
    assert masks.get_state_at_cell(95, 50) == "Missouri"
    assert masks.get_state_at_cell(120, 12) is None

    print("Generated state mask tests passed.")


if __name__ == "__main__":
    main()

from __future__ import annotations

from typing import Dict, Optional, Sequence

from state_polygons import STATE_POLYGONS, get_state_at_grid_cell


QUIZ_STATES: Sequence[str] = ("Washington", "California", "Texas")


class USStatesQuizModel:
    def __init__(self) -> None:
        self.state_order = list(QUIZ_STATES)
        self.completed_states: set[str] = set()
        self.current_index = 0
        self.target_state: Optional[str] = None
        self.status_text = "Ready"
        self.last_clicked_state: Optional[str] = None
        self.reset()

    def reset(self) -> None:
        self.completed_states.clear()
        self.current_index = 0
        self.target_state = self.state_order[0] if self.state_order else None
        self.status_text = "Ready"
        self.last_clicked_state = None

    def next_prompt(self) -> Optional[str]:
        if len(self.completed_states) >= len(self.state_order):
            self.target_state = None
            self.status_text = "Done"
            return None

        self.current_index += 1
        if self.current_index >= len(self.state_order):
            self.target_state = None
            self.status_text = "Done"
            return None

        self.target_state = self.state_order[self.current_index]
        return self.target_state

    def get_state_at_grid_cell(self, grid_x: int, grid_y: int) -> Optional[str]:
        return get_state_at_grid_cell(grid_x, grid_y)

    def check_click(self, grid_x: int, grid_y: int) -> Optional[str]:
        clicked_state = self.get_state_at_grid_cell(grid_x, grid_y)
        self.last_clicked_state = clicked_state

        if clicked_state is None:
            self.status_text = "Try again"
            return None

        if clicked_state == self.target_state:
            self.completed_states.add(clicked_state)
            self.status_text = "Correct"
            self.next_prompt()
        else:
            self.status_text = "Try again"

        return clicked_state

    def get_completed_polygons(self) -> Dict[str, list[tuple[int, int]]]:
        return {
            state_name: STATE_POLYGONS[state_name]
            for state_name in self.completed_states
            if state_name in STATE_POLYGONS
        }

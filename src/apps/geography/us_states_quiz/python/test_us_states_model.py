from us_states_model import USStatesQuizModel


def main() -> None:
    model = USStatesQuizModel()

    assert model.target_state == "Washington"

    assert model.get_state_at_grid_cell(12, 7) == "Washington"
    assert model.get_state_at_grid_cell(8, 45) == "California"
    assert model.get_state_at_grid_cell(70, 80) == "Texas"
    assert model.get_state_at_grid_cell(120, 12) is None

    clicked = model.check_click(12, 7)
    assert clicked == "Washington"
    assert "Washington" in model.completed_states
    assert model.target_state == "California"

    wrong_click = model.check_click(70, 80)
    assert wrong_click == "Texas"
    assert "California" not in model.completed_states
    assert model.target_state == "California"

    clicked = model.check_click(8, 45)
    assert clicked == "California"
    assert "California" in model.completed_states
    assert model.target_state == "Texas"

    clicked = model.check_click(70, 80)
    assert clicked == "Texas"
    assert "Texas" in model.completed_states
    assert model.target_state is None
    assert model.status_text == "Done"

    model.reset()
    assert model.target_state == "Washington"
    assert len(model.completed_states) == 0

    print("US states model tests passed.")


if __name__ == "__main__":
    main()

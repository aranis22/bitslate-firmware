from periodic_model import PeriodicTableModel


def test_all_118_elements_loaded():
    model = PeriodicTableModel()
    assert len(model.all_elements()) == 118


def test_carbon_exists_and_can_be_selected():
    model = PeriodicTableModel()
    carbon = model.get_element_by_atomic_number(6)
    assert carbon is not None
    assert carbon["symbol"] == "C"
    assert carbon["name"] == "Carbon"
    assert model.select_element(6)
    assert model.selected_element()["symbol"] == "C"


def test_hydrogen_position_exists():
    model = PeriodicTableModel()
    hydrogen = model.get_element_by_grid_position(1, 1)
    assert hydrogen is not None
    assert hydrogen["symbol"] == "H"


def test_helium_position_exists():
    model = PeriodicTableModel()
    helium = model.get_element_by_grid_position(18, 1)
    assert helium is not None
    assert helium["symbol"] == "He"


def test_selected_element_changes():
    model = PeriodicTableModel()
    assert model.selected_element()["symbol"] == "C"
    assert model.select_element(79)
    assert model.selected_element()["symbol"] == "Au"


def test_unknown_grid_position_returns_none():
    model = PeriodicTableModel()
    assert model.get_element_by_grid_position(3, 1) is None


if __name__ == "__main__":
    test_all_118_elements_loaded()
    test_carbon_exists_and_can_be_selected()
    test_hydrogen_position_exists()
    test_helium_position_exists()
    test_selected_element_changes()
    test_unknown_grid_position_returns_none()
    print("Periodic Table model tests passed")


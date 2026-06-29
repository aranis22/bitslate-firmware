from periodic_model import PeriodicTableModel


def test_placeholder_model_formats_unknown_values():
    model = PeriodicTableModel()
    assert model.format_value(None) == "-"
    assert model.format_value("Carbon") == "Carbon"


if __name__ == "__main__":
    test_placeholder_model_formats_unknown_values()
    print("Periodic Table placeholder tests passed")


from circuit_model import CircuitModel, ComponentType


def terminal(component, name):
    return next(item for item in component.terminals if item.name == name)


def test_empty_circuit_bulb_off():
    model = CircuitModel()
    assert not model.is_bulb_lit()
    assert model.get_status_text() == "Select Battery/Bulb/Wire"


def test_battery_only_bulb_off():
    model = CircuitModel()
    model.add_component(ComponentType.BATTERY, 120, 100)
    assert not model.is_bulb_lit()
    assert model.get_status_text() == "Circuit incomplete"


def test_battery_and_bulb_no_wires_bulb_off():
    model = CircuitModel()
    model.add_component(ComponentType.BATTERY, 120, 100)
    model.add_component(ComponentType.BULB, 260, 100)
    assert not model.is_bulb_lit()


def test_one_wire_bulb_off():
    model = CircuitModel()
    battery = model.add_component(ComponentType.BATTERY, 120, 100)
    bulb = model.add_component(ComponentType.BULB, 260, 100)

    assert model.add_wire(terminal(battery, "positive").id, terminal(bulb, "a").id)
    assert not model.is_bulb_lit()


def test_two_correct_wires_close_loop_bulb_on():
    model = CircuitModel()
    battery = model.add_component(ComponentType.BATTERY, 120, 100)
    bulb = model.add_component(ComponentType.BULB, 260, 100)

    assert model.add_wire(terminal(battery, "positive").id, terminal(bulb, "a").id)
    assert model.add_wire(terminal(bulb, "b").id, terminal(battery, "negative").id)
    assert model.is_bulb_lit()
    assert model.is_bulb_lit(bulb.id)
    assert model.get_status_text() == "Circuit complete: bulb ON"


def test_clear_resets_circuit_bulb_off():
    model = CircuitModel()
    battery = model.add_component(ComponentType.BATTERY, 120, 100)
    bulb = model.add_component(ComponentType.BULB, 260, 100)
    model.add_wire(terminal(battery, "positive").id, terminal(bulb, "a").id)
    model.add_wire(terminal(bulb, "b").id, terminal(battery, "negative").id)

    assert model.is_bulb_lit()
    model.clear()
    assert not model.is_bulb_lit()
    assert len(model.components) == 0
    assert len(model.wires) == 0


if __name__ == "__main__":
    test_empty_circuit_bulb_off()
    test_battery_only_bulb_off()
    test_battery_and_bulb_no_wires_bulb_off()
    test_one_wire_bulb_off()
    test_two_correct_wires_close_loop_bulb_on()
    test_clear_resets_circuit_bulb_off()
    print("CircuitModel Python tests passed")

#include "CircuitModel.h"

#include <cassert>
#include <iostream>

static void testEmptyOff() {
    CircuitModel model;
    assert(!model.isBulbLit());
    assert(model.getComponentCount() == 0);
    assert(model.getWireCount() == 0);
}

static void testBatteryBulbNoWiresOff() {
    CircuitModel model;
    model.addComponent(ComponentType::Battery, 120, 100);
    model.addComponent(ComponentType::Bulb, 260, 100);
    assert(!model.isBulbLit());
}

static void testOneWireOff() {
    CircuitModel model;
    const int battery = model.addComponent(ComponentType::Battery, 120, 100);
    const int bulb = model.addComponent(ComponentType::Bulb, 260, 100);

    assert(model.addWire(model.getTerminalId(battery, "positive"), model.getTerminalId(bulb, "a")));
    assert(!model.isBulbLit());
}

static void testTwoWiresOn() {
    CircuitModel model;
    const int battery = model.addComponent(ComponentType::Battery, 120, 100);
    const int bulb = model.addComponent(ComponentType::Bulb, 260, 100);

    assert(model.addWire(model.getTerminalId(battery, "positive"), model.getTerminalId(bulb, "a")));
    assert(model.addWire(model.getTerminalId(bulb, "b"), model.getTerminalId(battery, "negative")));
    assert(model.isBulbLit());
    assert(model.isBulbLit(bulb));
}

static void testClearOff() {
    CircuitModel model;
    const int battery = model.addComponent(ComponentType::Battery, 120, 100);
    const int bulb = model.addComponent(ComponentType::Bulb, 260, 100);

    model.addWire(model.getTerminalId(battery, "positive"), model.getTerminalId(bulb, "a"));
    model.addWire(model.getTerminalId(bulb, "b"), model.getTerminalId(battery, "negative"));
    assert(model.isBulbLit());

    model.clear();
    assert(!model.isBulbLit());
    assert(model.getComponentCount() == 0);
    assert(model.getWireCount() == 0);
}

int main() {
    testEmptyOff();
    testBatteryBulbNoWiresOff();
    testOneWireOff();
    testTwoWiresOn();
    testClearOff();
    std::cout << "CircuitModel C++ tests passed\n";
    return 0;
}

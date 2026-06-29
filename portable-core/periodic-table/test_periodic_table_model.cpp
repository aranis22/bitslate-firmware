#include "PeriodicTableModel.h"

#include <cassert>
#include <cstring>
#include <iostream>

namespace {

void testElementCount() {
    PeriodicTableModel model;
    assert(model.getElementCount() == 118);
}

void testDefaultSelectionIsCarbon() {
    PeriodicTableModel model;
    const PeriodicElement* selected = model.getSelectedElement();
    assert(selected != nullptr);
    assert(selected->atomicNumber == 6);
    assert(std::strcmp(selected->symbol, "C") == 0);
    assert(std::strcmp(selected->name, "Carbon") == 0);
}

void testHydrogenPosition() {
    PeriodicTableModel model;
    const PeriodicElement* hydrogen = model.getElementByGridPosition(1, 1);
    assert(hydrogen != nullptr);
    assert(hydrogen->atomicNumber == 1);
    assert(std::strcmp(hydrogen->symbol, "H") == 0);
}

void testHeliumPosition() {
    PeriodicTableModel model;
    const PeriodicElement* helium = model.getElementByGridPosition(18, 1);
    assert(helium != nullptr);
    assert(helium->atomicNumber == 2);
    assert(std::strcmp(helium->symbol, "He") == 0);
}

void testSelectionByAtomicNumber() {
    PeriodicTableModel model;
    assert(model.selectByAtomicNumber(3));
    assert(std::strcmp(model.getSelectedElement()->symbol, "Li") == 0);
    assert(model.selectByAtomicNumber(79));
    assert(std::strcmp(model.getSelectedElement()->symbol, "Au") == 0);
    assert(model.selectByAtomicNumber(92));
    assert(std::strcmp(model.getSelectedElement()->symbol, "U") == 0);
}

void testInvalidSelection() {
    PeriodicTableModel model;
    assert(model.getElementByAtomicNumber(0) == nullptr);
    assert(model.getElementByAtomicNumber(999) == nullptr);
    assert(!model.selectByAtomicNumber(0));
    assert(!model.selectByAtomicNumber(999));
    assert(model.getElementByGridPosition(3, 1) == nullptr);
    assert(model.getElementByGridPosition(40, 40) == nullptr);
    assert(!model.selectByGridPosition(3, 1));
    assert(!model.selectByGridPosition(40, 40));
}

}  // namespace

int main() {
    testElementCount();
    testDefaultSelectionIsCarbon();
    testHydrogenPosition();
    testHeliumPosition();
    testSelectionByAtomicNumber();
    testInvalidSelection();

    std::cout << "Periodic Table C++ core tests passed\n";
    return 0;
}


#include "AtomModel.h"

#include <cassert>
#include <cstring>
#include <iostream>

namespace {
void addManyProtons(AtomModel& atom, int count) {
    for (int i = 0; i < count; ++i) {
        assert(atom.addProton());
    }
}

void addManyNeutrons(AtomModel& atom, int count) {
    for (int i = 0; i < count; ++i) {
        assert(atom.addNeutron());
    }
}

void addManyElectrons(AtomModel& atom, int count) {
    for (int i = 0; i < count; ++i) {
        assert(atom.addElectron());
    }
}

void testEmptyAtomUnknown() {
    AtomModel atom;

    assert(atom.getProtons() == 0);
    assert(atom.getNeutrons() == 0);
    assert(atom.getElectrons() == 0);
    assert(atom.getAtomicNumber() == 0);
    assert(atom.getMassNumber() == 0);
    assert(atom.getCharge() == 0);
    assert(atom.isNeutral());
    assert(std::strcmp(atom.getElementSymbol(), "?") == 0);
    assert(std::strcmp(atom.getElementName(), "Unknown") == 0);
}

void testHydrogenNeutral() {
    AtomModel atom;
    assert(atom.addProton());
    assert(atom.addElectron());

    assert(atom.getAtomicNumber() == 1);
    assert(atom.getMassNumber() == 1);
    assert(atom.getCharge() == 0);
    assert(atom.isNeutral());
    assert(std::strcmp(atom.getElementSymbol(), "H") == 0);
    assert(std::strcmp(atom.getElementName(), "Hydrogen") == 0);
}

void testHeliumMassAndCharge() {
    AtomModel atom;
    addManyProtons(atom, 2);
    addManyNeutrons(atom, 2);
    addManyElectrons(atom, 2);

    assert(atom.getAtomicNumber() == 2);
    assert(atom.getMassNumber() == 4);
    assert(atom.getCharge() == 0);
    assert(atom.isNeutral());
    assert(std::strcmp(atom.getElementSymbol(), "He") == 0);
}

void testLithiumIon() {
    AtomModel atom;
    addManyProtons(atom, 3);
    addManyNeutrons(atom, 4);
    addManyElectrons(atom, 2);

    assert(atom.getAtomicNumber() == 3);
    assert(atom.getMassNumber() == 7);
    assert(atom.getCharge() == 1);
    assert(!atom.isNeutral());
    assert(std::strcmp(atom.getElementSymbol(), "Li") == 0);
}

void testElectronShellCapacity() {
    AtomModel atom;
    addManyElectrons(atom, 10);

    assert(atom.getElectrons() == 10);
    assert(!atom.addElectron());
    assert(atom.getElectrons() == 10);
}

void testResetWorks() {
    AtomModel atom;
    addManyProtons(atom, 4);
    addManyNeutrons(atom, 5);
    addManyElectrons(atom, 3);

    atom.reset();

    assert(atom.getProtons() == 0);
    assert(atom.getNeutrons() == 0);
    assert(atom.getElectrons() == 0);
    assert(atom.getMassNumber() == 0);
    assert(atom.getCharge() == 0);
    assert(std::strcmp(atom.getElementSymbol(), "?") == 0);
}
}

int main() {
    testEmptyAtomUnknown();
    testHydrogenNeutral();
    testHeliumMassAndCharge();
    testLithiumIon();
    testElectronShellCapacity();
    testResetWorks();

    std::cout << "Build an Atom C++ core tests passed\n";
    return 0;
}


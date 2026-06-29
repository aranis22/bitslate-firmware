#include "AtomModel.h"

namespace {
constexpr const char* ElementSymbols[] = {
    "?",
    "H",
    "He",
    "Li",
    "Be",
    "B",
    "C",
    "N",
    "O",
    "F",
    "Ne",
};

constexpr const char* ElementNames[] = {
    "Unknown",
    "Hydrogen",
    "Helium",
    "Lithium",
    "Beryllium",
    "Boron",
    "Carbon",
    "Nitrogen",
    "Oxygen",
    "Fluorine",
    "Neon",
};
}

void AtomModel::reset() {
    protons = 0;
    neutrons = 0;
    electrons = 0;
}

bool AtomModel::addProton() {
    if (protons >= MaxProtons) {
        return false;
    }

    ++protons;
    return true;
}

bool AtomModel::addNeutron() {
    if (neutrons >= MaxNeutrons) {
        return false;
    }

    ++neutrons;
    return true;
}

bool AtomModel::addElectron() {
    if (electrons >= ShellOneCapacity + ShellTwoCapacity) {
        return false;
    }

    ++electrons;
    return true;
}

bool AtomModel::removeProton() {
    if (protons <= 0) {
        return false;
    }

    --protons;
    return true;
}

bool AtomModel::removeNeutron() {
    if (neutrons <= 0) {
        return false;
    }

    --neutrons;
    return true;
}

bool AtomModel::removeElectron() {
    if (electrons <= 0) {
        return false;
    }

    --electrons;
    return true;
}

int AtomModel::getProtons() const {
    return protons;
}

int AtomModel::getNeutrons() const {
    return neutrons;
}

int AtomModel::getElectrons() const {
    return electrons;
}

int AtomModel::getAtomicNumber() const {
    return protons;
}

int AtomModel::getMassNumber() const {
    return protons + neutrons;
}

int AtomModel::getCharge() const {
    return protons - electrons;
}

const char* AtomModel::getElementSymbol() const {
    return symbolForProtons(protons);
}

const char* AtomModel::getElementName() const {
    return nameForProtons(protons);
}

bool AtomModel::isNeutral() const {
    return getCharge() == 0;
}

const char* AtomModel::symbolForProtons(int count) {
    if (count < 0 || count > MaxProtons) {
        return "?";
    }

    return ElementSymbols[count];
}

const char* AtomModel::nameForProtons(int count) {
    if (count < 0 || count > MaxProtons) {
        return "Unknown";
    }

    return ElementNames[count];
}


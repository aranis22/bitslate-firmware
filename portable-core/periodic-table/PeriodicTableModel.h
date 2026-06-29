#pragma once

#include "PeriodicElement.h"

class PeriodicTableModel {
public:
    PeriodicTableModel();

    const PeriodicElement* getAllElements() const;
    int getElementCount() const;

    const PeriodicElement* getElementByAtomicNumber(int atomicNumber) const;
    const PeriodicElement* getElementByGridPosition(int col, int row) const;

    bool selectByAtomicNumber(int atomicNumber);
    bool selectByGridPosition(int col, int row);
    const PeriodicElement* getSelectedElement() const;

private:
    int selectedAtomicNumber;
};


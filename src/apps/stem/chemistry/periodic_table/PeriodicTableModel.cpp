#include "PeriodicTableModel.h"

namespace {
const PeriodicElement kElements[] = {
    {1, "H", "Hydrogen", "1.008", "Nonmetal", 1, 1, 1, 1, "1", "Gas", "2.20"},
    {2, "He", "Helium", "4.0026", "Noble gas", 18, 1, 18, 1, "2", "Gas", "-"},
    {3, "Li", "Lithium", "6.94", "Alkali metal", 1, 2, 1, 2, "2, 1", "Solid", "0.98"},
    {4, "Be", "Beryllium", "9.0122", "Alkaline earth metal", 2, 2, 2, 2, "2, 2", "Solid", "1.57"},
    {5, "B", "Boron", "10.81", "Metalloid", 13, 2, 13, 2, "2, 3", "Solid", "2.04"},
    {6, "C", "Carbon", "12.011", "Nonmetal", 14, 2, 14, 2, "2, 4", "Solid", "2.55"},
    {7, "N", "Nitrogen", "14.007", "Nonmetal", 15, 2, 15, 2, "2, 5", "Gas", "3.04"},
    {8, "O", "Oxygen", "15.999", "Nonmetal", 16, 2, 16, 2, "2, 6", "Gas", "3.44"},
    {9, "F", "Fluorine", "18.998", "Halogen", 17, 2, 17, 2, "2, 7", "Gas", "3.98"},
    {10, "Ne", "Neon", "20.180", "Noble gas", 18, 2, 18, 2, "2, 8", "Gas", "-"},
    {11, "Na", "Sodium", "22.990", "Alkali metal", 1, 3, 1, 3, "2, 8, 1", "Solid", "0.93"},
    {12, "Mg", "Magnesium", "24.305", "Alkaline earth metal", 2, 3, 2, 3, "2, 8, 2", "Solid", "1.31"},
    {13, "Al", "Aluminium", "26.982", "Post-transition metal", 13, 3, 13, 3, "2, 8, 3", "Solid", "1.61"},
    {14, "Si", "Silicon", "28.085", "Metalloid", 14, 3, 14, 3, "2, 8, 4", "Solid", "1.90"},
    {15, "P", "Phosphorus", "30.974", "Nonmetal", 15, 3, 15, 3, "2, 8, 5", "Solid", "2.19"},
    {16, "S", "Sulfur", "32.06", "Nonmetal", 16, 3, 16, 3, "2, 8, 6", "Solid", "2.58"},
    {17, "Cl", "Chlorine", "35.45", "Halogen", 17, 3, 17, 3, "2, 8, 7", "Gas", "3.16"},
    {18, "Ar", "Argon", "39.948", "Noble gas", 18, 3, 18, 3, "2, 8, 8", "Gas", "-"},
    {19, "K", "Potassium", "39.098", "Alkali metal", 1, 4, 1, 4, "2, 8, 8, 1", "Solid", "0.82"},
    {20, "Ca", "Calcium", "40.078", "Alkaline earth metal", 2, 4, 2, 4, "2, 8, 8, 2", "Solid", "1.00"},
    {21, "Sc", "Scandium", "44.956", "Transition metal", 3, 4, 3, 4, "2, 8, 9, 2", "Solid", "1.36"},
    {22, "Ti", "Titanium", "47.867", "Transition metal", 4, 4, 4, 4, "2, 8, 10, 2", "Solid", "1.54"},
    {23, "V", "Vanadium", "50.942", "Transition metal", 5, 4, 5, 4, "2, 8, 11, 2", "Solid", "1.63"},
    {24, "Cr", "Chromium", "51.996", "Transition metal", 6, 4, 6, 4, "2, 8, 13, 1", "Solid", "1.66"},
    {25, "Mn", "Manganese", "54.938", "Transition metal", 7, 4, 7, 4, "2, 8, 13, 2", "Solid", "1.55"},
    {26, "Fe", "Iron", "55.845", "Transition metal", 8, 4, 8, 4, "2, 8, 14, 2", "Solid", "1.83"},
    {27, "Co", "Cobalt", "58.933", "Transition metal", 9, 4, 9, 4, "2, 8, 15, 2", "Solid", "1.88"},
    {28, "Ni", "Nickel", "58.693", "Transition metal", 10, 4, 10, 4, "2, 8, 16, 2", "Solid", "1.91"},
    {29, "Cu", "Copper", "63.546", "Transition metal", 11, 4, 11, 4, "2, 8, 18, 1", "Solid", "1.90"},
    {30, "Zn", "Zinc", "65.38", "Transition metal", 12, 4, 12, 4, "2, 8, 18, 2", "Solid", "1.65"},
    {31, "Ga", "Gallium", "69.723", "Post-transition metal", 13, 4, 13, 4, "2, 8, 18, 3", "Solid", "1.81"},
    {32, "Ge", "Germanium", "72.630", "Metalloid", 14, 4, 14, 4, "2, 8, 18, 4", "Solid", "2.01"},
    {33, "As", "Arsenic", "74.922", "Metalloid", 15, 4, 15, 4, "2, 8, 18, 5", "Solid", "2.18"},
    {34, "Se", "Selenium", "78.971", "Nonmetal", 16, 4, 16, 4, "2, 8, 18, 6", "Solid", "2.55"},
    {35, "Br", "Bromine", "79.904", "Halogen", 17, 4, 17, 4, "2, 8, 18, 7", "Liquid", "2.96"},
    {36, "Kr", "Krypton", "83.798", "Noble gas", 18, 4, 18, 4, "2, 8, 18, 8", "Gas", "3.00"},
    {37, "Rb", "Rubidium", "85.468", "Alkali metal", 1, 5, 1, 5, "2, 8, 18, 8, 1", "Solid", "0.82"},
    {38, "Sr", "Strontium", "87.62", "Alkaline earth metal", 2, 5, 2, 5, "2, 8, 18, 8, 2", "Solid", "0.95"},
    {39, "Y", "Yttrium", "88.906", "Transition metal", 3, 5, 3, 5, "2, 8, 18, 9, 2", "Solid", "1.22"},
    {40, "Zr", "Zirconium", "91.224", "Transition metal", 4, 5, 4, 5, "2, 8, 18, 10, 2", "Solid", "1.33"},
    {41, "Nb", "Niobium", "92.906", "Transition metal", 5, 5, 5, 5, "2, 8, 18, 12, 1", "Solid", "1.6"},
    {42, "Mo", "Molybdenum", "95.95", "Transition metal", 6, 5, 6, 5, "2, 8, 18, 13, 1", "Solid", "2.16"},
    {43, "Tc", "Technetium", "(98)", "Transition metal", 7, 5, 7, 5, "2, 8, 18, 13, 2", "Solid", "1.9"},
    {44, "Ru", "Ruthenium", "101.07", "Transition metal", 8, 5, 8, 5, "2, 8, 18, 15, 1", "Solid", "2.2"},
    {45, "Rh", "Rhodium", "102.91", "Transition metal", 9, 5, 9, 5, "2, 8, 18, 16, 1", "Solid", "2.28"},
    {46, "Pd", "Palladium", "106.42", "Transition metal", 10, 5, 10, 5, "2, 8, 18, 18", "Solid", "2.20"},
    {47, "Ag", "Silver", "107.87", "Transition metal", 11, 5, 11, 5, "2, 8, 18, 18, 1", "Solid", "1.93"},
    {48, "Cd", "Cadmium", "112.41", "Transition metal", 12, 5, 12, 5, "2, 8, 18, 18, 2", "Solid", "1.69"},
    {49, "In", "Indium", "114.82", "Post-transition metal", 13, 5, 13, 5, "2, 8, 18, 18, 3", "Solid", "1.78"},
    {50, "Sn", "Tin", "118.71", "Post-transition metal", 14, 5, 14, 5, "2, 8, 18, 18, 4", "Solid", "1.96"},
    {51, "Sb", "Antimony", "121.76", "Metalloid", 15, 5, 15, 5, "2, 8, 18, 18, 5", "Solid", "2.05"},
    {52, "Te", "Tellurium", "127.60", "Metalloid", 16, 5, 16, 5, "2, 8, 18, 18, 6", "Solid", "2.1"},
    {53, "I", "Iodine", "126.90", "Halogen", 17, 5, 17, 5, "2, 8, 18, 18, 7", "Solid", "2.66"},
    {54, "Xe", "Xenon", "131.29", "Noble gas", 18, 5, 18, 5, "2, 8, 18, 18, 8", "Gas", "2.60"},
    {55, "Cs", "Caesium", "132.91", "Alkali metal", 1, 6, 1, 6, "2, 8, 18, 18, 8, 1", "Solid", "0.79"},
    {56, "Ba", "Barium", "137.33", "Alkaline earth metal", 2, 6, 2, 6, "2, 8, 18, 18, 8, 2", "Solid", "0.89"},
    {57, "La", "Lanthanum", "138.91", "Lanthanide", 0, 6, 4, 8, "2, 8, 18, 18, 9, 2", "Solid", "1.10"},
    {58, "Ce", "Cerium", "140.12", "Lanthanide", 0, 6, 5, 8, "2, 8, 18, 19, 9, 2", "Solid", "1.12"},
    {59, "Pr", "Praseodymium", "140.91", "Lanthanide", 0, 6, 6, 8, "2, 8, 18, 21, 8, 2", "Solid", "1.13"},
    {60, "Nd", "Neodymium", "144.24", "Lanthanide", 0, 6, 7, 8, "2, 8, 18, 22, 8, 2", "Solid", "1.14"},
    {61, "Pm", "Promethium", "(145)", "Lanthanide", 0, 6, 8, 8, "2, 8, 18, 23, 8, 2", "Solid", "1.13"},
    {62, "Sm", "Samarium", "150.36", "Lanthanide", 0, 6, 9, 8, "2, 8, 18, 24, 8, 2", "Solid", "1.17"},
    {63, "Eu", "Europium", "151.96", "Lanthanide", 0, 6, 10, 8, "2, 8, 18, 25, 8, 2", "Solid", "1.2"},
    {64, "Gd", "Gadolinium", "157.25", "Lanthanide", 0, 6, 11, 8, "2, 8, 18, 25, 9, 2", "Solid", "1.2"},
    {65, "Tb", "Terbium", "158.93", "Lanthanide", 0, 6, 12, 8, "2, 8, 18, 27, 8, 2", "Solid", "1.2"},
    {66, "Dy", "Dysprosium", "162.50", "Lanthanide", 0, 6, 13, 8, "2, 8, 18, 28, 8, 2", "Solid", "1.22"},
    {67, "Ho", "Holmium", "164.93", "Lanthanide", 0, 6, 14, 8, "2, 8, 18, 29, 8, 2", "Solid", "1.23"},
    {68, "Er", "Erbium", "167.26", "Lanthanide", 0, 6, 15, 8, "2, 8, 18, 30, 8, 2", "Solid", "1.24"},
    {69, "Tm", "Thulium", "168.93", "Lanthanide", 0, 6, 16, 8, "2, 8, 18, 31, 8, 2", "Solid", "1.25"},
    {70, "Yb", "Ytterbium", "173.05", "Lanthanide", 0, 6, 17, 8, "2, 8, 18, 32, 8, 2", "Solid", "1.1"},
    {71, "Lu", "Lutetium", "174.97", "Lanthanide", 3, 6, 18, 8, "2, 8, 18, 32, 9, 2", "Solid", "1.27"},
    {72, "Hf", "Hafnium", "178.49", "Transition metal", 4, 6, 4, 6, "2, 8, 18, 32, 10, 2", "Solid", "1.3"},
    {73, "Ta", "Tantalum", "180.95", "Transition metal", 5, 6, 5, 6, "2, 8, 18, 32, 11, 2", "Solid", "1.5"},
    {74, "W", "Tungsten", "183.84", "Transition metal", 6, 6, 6, 6, "2, 8, 18, 32, 12, 2", "Solid", "2.36"},
    {75, "Re", "Rhenium", "186.21", "Transition metal", 7, 6, 7, 6, "2, 8, 18, 32, 13, 2", "Solid", "1.9"},
    {76, "Os", "Osmium", "190.23", "Transition metal", 8, 6, 8, 6, "2, 8, 18, 32, 14, 2", "Solid", "2.2"},
    {77, "Ir", "Iridium", "192.22", "Transition metal", 9, 6, 9, 6, "2, 8, 18, 32, 15, 2", "Solid", "2.20"},
    {78, "Pt", "Platinum", "195.08", "Transition metal", 10, 6, 10, 6, "2, 8, 18, 32, 17, 1", "Solid", "2.28"},
    {79, "Au", "Gold", "196.97", "Transition metal", 11, 6, 11, 6, "2, 8, 18, 32, 18, 1", "Solid", "2.54"},
    {80, "Hg", "Mercury", "200.59", "Transition metal", 12, 6, 12, 6, "2, 8, 18, 32, 18, 2", "Liquid", "2.00"},
    {81, "Tl", "Thallium", "204.38", "Post-transition metal", 13, 6, 13, 6, "2, 8, 18, 32, 18, 3", "Solid", "1.62"},
    {82, "Pb", "Lead", "207.2", "Post-transition metal", 14, 6, 14, 6, "2, 8, 18, 32, 18, 4", "Solid", "2.33"},
    {83, "Bi", "Bismuth", "208.98", "Post-transition metal", 15, 6, 15, 6, "2, 8, 18, 32, 18, 5", "Solid", "2.02"},
    {84, "Po", "Polonium", "(209)", "Post-transition metal", 16, 6, 16, 6, "2, 8, 18, 32, 18, 6", "Solid", "2.0"},
    {85, "At", "Astatine", "(210)", "Halogen", 17, 6, 17, 6, "2, 8, 18, 32, 18, 7", "Solid", "2.2"},
    {86, "Rn", "Radon", "(222)", "Noble gas", 18, 6, 18, 6, "2, 8, 18, 32, 18, 8", "Gas", "-"},
    {87, "Fr", "Francium", "(223)", "Alkali metal", 1, 7, 1, 7, "2, 8, 18, 32, 18, 8, 1", "Solid", "0.7"},
    {88, "Ra", "Radium", "(226)", "Alkaline earth metal", 2, 7, 2, 7, "2, 8, 18, 32, 18, 8, 2", "Solid", "0.9"},
    {89, "Ac", "Actinium", "(227)", "Actinide", 0, 7, 4, 9, "2, 8, 18, 32, 18, 9, 2", "Solid", "1.1"},
    {90, "Th", "Thorium", "232.04", "Actinide", 0, 7, 5, 9, "2, 8, 18, 32, 18, 10, 2", "Solid", "1.3"},
    {91, "Pa", "Protactinium", "231.04", "Actinide", 0, 7, 6, 9, "2, 8, 18, 32, 20, 9, 2", "Solid", "1.5"},
    {92, "U", "Uranium", "238.03", "Actinide", 0, 7, 7, 9, "2, 8, 18, 32, 21, 9, 2", "Solid", "1.38"},
    {93, "Np", "Neptunium", "(237)", "Actinide", 0, 7, 8, 9, "2, 8, 18, 32, 22, 9, 2", "Solid", "1.36"},
    {94, "Pu", "Plutonium", "(244)", "Actinide", 0, 7, 9, 9, "2, 8, 18, 32, 24, 8, 2", "Solid", "1.28"},
    {95, "Am", "Americium", "(243)", "Actinide", 0, 7, 10, 9, "2, 8, 18, 32, 25, 8, 2", "Solid", "1.3"},
    {96, "Cm", "Curium", "(247)", "Actinide", 0, 7, 11, 9, "2, 8, 18, 32, 25, 9, 2", "Solid", "1.3"},
    {97, "Bk", "Berkelium", "(247)", "Actinide", 0, 7, 12, 9, "2, 8, 18, 32, 27, 8, 2", "Solid", "1.3"},
    {98, "Cf", "Californium", "(251)", "Actinide", 0, 7, 13, 9, "2, 8, 18, 32, 28, 8, 2", "Solid", "1.3"},
    {99, "Es", "Einsteinium", "(252)", "Actinide", 0, 7, 14, 9, "2, 8, 18, 32, 29, 8, 2", "Solid", "1.3"},
    {100, "Fm", "Fermium", "(257)", "Actinide", 0, 7, 15, 9, "2, 8, 18, 32, 30, 8, 2", "Solid", "1.3"},
    {101, "Md", "Mendelevium", "(258)", "Actinide", 0, 7, 16, 9, "2, 8, 18, 32, 31, 8, 2", "Solid", "1.3"},
    {102, "No", "Nobelium", "(259)", "Actinide", 0, 7, 17, 9, "2, 8, 18, 32, 32, 8, 2", "Solid", "1.3"},
    {103, "Lr", "Lawrencium", "(266)", "Actinide", 3, 7, 18, 9, "2, 8, 18, 32, 32, 8, 3", "Solid", "1.3"},
    {104, "Rf", "Rutherfordium", "(267)", "Transition metal", 4, 7, 4, 7, "2, 8, 18, 32, 32, 10, 2", "Solid", "-"},
    {105, "Db", "Dubnium", "(268)", "Transition metal", 5, 7, 5, 7, "2, 8, 18, 32, 32, 11, 2", "Solid", "-"},
    {106, "Sg", "Seaborgium", "(269)", "Transition metal", 6, 7, 6, 7, "2, 8, 18, 32, 32, 12, 2", "Solid", "-"},
    {107, "Bh", "Bohrium", "(270)", "Transition metal", 7, 7, 7, 7, "2, 8, 18, 32, 32, 13, 2", "Solid", "-"},
    {108, "Hs", "Hassium", "(277)", "Transition metal", 8, 7, 8, 7, "2, 8, 18, 32, 32, 14, 2", "Solid", "-"},
    {109, "Mt", "Meitnerium", "(278)", "Unknown", 9, 7, 9, 7, "2, 8, 18, 32, 32, 15, 2", "-", "-"},
    {110, "Ds", "Darmstadtium", "(281)", "Unknown", 10, 7, 10, 7, "2, 8, 18, 32, 32, 17, 1", "-", "-"},
    {111, "Rg", "Roentgenium", "(282)", "Unknown", 11, 7, 11, 7, "2, 8, 18, 32, 32, 17, 2", "-", "-"},
    {112, "Cn", "Copernicium", "(285)", "Transition metal", 12, 7, 12, 7, "2, 8, 18, 32, 32, 18, 2", "-", "-"},
    {113, "Nh", "Nihonium", "(286)", "Unknown", 13, 7, 13, 7, "2, 8, 18, 32, 32, 18, 3", "-", "-"},
    {114, "Fl", "Flerovium", "(289)", "Post-transition metal", 14, 7, 14, 7, "2, 8, 18, 32, 32, 18, 4", "-", "-"},
    {115, "Mc", "Moscovium", "(290)", "Unknown", 15, 7, 15, 7, "2, 8, 18, 32, 32, 18, 5", "-", "-"},
    {116, "Lv", "Livermorium", "(293)", "Unknown", 16, 7, 16, 7, "2, 8, 18, 32, 32, 18, 6", "-", "-"},
    {117, "Ts", "Tennessine", "(294)", "Halogen", 17, 7, 17, 7, "2, 8, 18, 32, 32, 18, 7", "-", "-"},
    {118, "Og", "Oganesson", "(294)", "Noble gas", 18, 7, 18, 7, "2, 8, 18, 32, 32, 18, 8", "-", "-"},
};

constexpr int kElementCount = static_cast<int>(sizeof(kElements) / sizeof(kElements[0]));
}  // namespace

PeriodicTableModel::PeriodicTableModel() : selectedAtomicNumber(6) {}

const PeriodicElement* PeriodicTableModel::getAllElements() const {
  return kElements;
}

int PeriodicTableModel::getElementCount() const {
  return kElementCount;
}

const PeriodicElement* PeriodicTableModel::getElementByAtomicNumber(int atomicNumber) const {
  if (atomicNumber < 1 || atomicNumber > kElementCount) {
    return nullptr;
  }
  return &kElements[atomicNumber - 1];
}

const PeriodicElement* PeriodicTableModel::getElementByGridPosition(int col, int row) const {
  for (int i = 0; i < kElementCount; ++i) {
    if (kElements[i].gridCol == col && kElements[i].gridRow == row) {
      return &kElements[i];
    }
  }
  return nullptr;
}

bool PeriodicTableModel::selectByAtomicNumber(int atomicNumber) {
  if (getElementByAtomicNumber(atomicNumber) == nullptr) {
    return false;
  }
  selectedAtomicNumber = atomicNumber;
  return true;
}

bool PeriodicTableModel::selectByGridPosition(int col, int row) {
  const PeriodicElement* element = getElementByGridPosition(col, row);
  if (element == nullptr) {
    return false;
  }
  selectedAtomicNumber = element->atomicNumber;
  return true;
}

const PeriodicElement* PeriodicTableModel::getSelectedElement() const {
  return getElementByAtomicNumber(selectedAtomicNumber);
}


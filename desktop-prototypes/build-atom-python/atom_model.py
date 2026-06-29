ELEMENTS = [
    ("?", "No element"),
    ("H", "Hydrogen"),
    ("He", "Helium"),
    ("Li", "Lithium"),
    ("Be", "Beryllium"),
    ("B", "Boron"),
    ("C", "Carbon"),
    ("N", "Nitrogen"),
    ("O", "Oxygen"),
    ("F", "Fluorine"),
    ("Ne", "Neon"),
]

MAX_PROTONS = 10
MAX_ELECTRONS = 10


class AtomModel:
    """Minimal model for the Build an Atom desktop prototype."""

    def __init__(self):
        self.reset()

    def add_proton(self):
        if self.protons < MAX_PROTONS:
            self.protons += 1

    def add_neutron(self):
        self.neutrons += 1

    def add_electron(self):
        if self.electrons < MAX_ELECTRONS:
            self.electrons += 1

    def reset(self):
        self.protons = 0
        self.neutrons = 0
        self.electrons = 0

    def atomic_number(self):
        return self.protons

    def mass_number(self):
        return self.protons + self.neutrons

    def charge(self):
        return self.protons - self.electrons

    def element_symbol(self):
        if 0 <= self.protons < len(ELEMENTS):
            return ELEMENTS[self.protons][0]
        return "?"

    def element_name(self):
        if 0 <= self.protons < len(ELEMENTS):
            return ELEMENTS[self.protons][1]
        return "Unknown"

    def electron_shell_counts(self):
        shell_one = min(self.electrons, 2)
        shell_two = max(0, min(self.electrons - shell_one, 8))
        return shell_one, shell_two

from atom_model import AtomModel


def test_empty_atom():
    atom = AtomModel()
    assert atom.atomic_number() == 0
    assert atom.mass_number() == 0
    assert atom.charge() == 0
    assert atom.element_symbol() == "?"


def test_helium_like_atom():
    atom = AtomModel()
    atom.add_proton()
    atom.add_proton()
    atom.add_neutron()
    atom.add_neutron()
    atom.add_electron()
    atom.add_electron()

    assert atom.atomic_number() == 2
    assert atom.mass_number() == 4
    assert atom.charge() == 0
    assert atom.element_symbol() == "He"
    assert atom.element_name() == "Helium"


def test_lithium_positive_ion():
    atom = AtomModel()
    for _ in range(3):
        atom.add_proton()
    for _ in range(4):
        atom.add_neutron()
    for _ in range(2):
        atom.add_electron()

    assert atom.element_symbol() == "Li"
    assert atom.mass_number() == 7
    assert atom.charge() == 1
    assert atom.electron_shell_counts() == (2, 0)


if __name__ == "__main__":
    test_empty_atom()
    test_helium_like_atom()
    test_lithium_positive_ion()
    print("Build an Atom model tests passed")


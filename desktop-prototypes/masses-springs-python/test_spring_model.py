from spring_model import SpringMassModel


def approx(a, b, tolerance=1e-6):
    return abs(a - b) <= tolerance


def test_initializes():
    model = SpringMassModel()
    assert model.mass_kg == 1.0
    assert model.spring_k == 80.0
    assert model.playing is True
    assert model.dragging is False


def test_mass_increases_equilibrium_extension():
    model = SpringMassModel()
    base = model.get_equilibrium_extension()
    model.set_mass(3.0)
    assert model.get_equilibrium_extension() > base


def test_spring_constant_decreases_equilibrium_extension():
    model = SpringMassModel()
    model.set_spring_k(40.0)
    loose = model.get_equilibrium_extension()
    model.set_spring_k(160.0)
    stiff = model.get_equilibrium_extension()
    assert stiff < loose


def test_damping_reduces_velocity():
    model = SpringMassModel()
    model.set_damping(8.0)
    model.displacement = model.get_equilibrium_extension()
    model.velocity = 4.0
    model.step(1.0 / 60.0)
    assert model.velocity < 4.0


def test_reset_restores_initial_state():
    model = SpringMassModel()
    model.set_mass(4.0)
    model.set_spring_k(120.0)
    model.displacement = 1.0
    model.velocity = -3.0
    model.reset()
    assert model.mass_kg == model.default_mass_kg
    assert model.spring_k == model.default_spring_k
    assert model.damping == model.default_damping
    assert approx(model.displacement, model.get_equilibrium_extension())
    assert model.velocity == 0.0


def test_dragging_changes_displacement():
    model = SpringMassModel()
    model.start_drag(model.anchor_y + model.rest_length + 120)
    assert model.dragging is True
    assert model.playing is False
    assert model.displacement > 0.5


def test_release_resumes_physics():
    model = SpringMassModel()
    model.start_drag(model.anchor_y + model.rest_length + 120)
    model.release_drag()
    assert model.dragging is False
    assert model.playing is True


def test_no_damping_stays_bounded_short_term():
    model = SpringMassModel()
    model.set_damping(0.0)
    model.displacement = model.get_equilibrium_extension() + 0.45
    for _ in range(600):
        model.step(1.0 / 60.0)
    assert -0.35 <= model.displacement <= 1.25
    assert abs(model.velocity) < 10.1


def test_damping_settles_toward_equilibrium():
    model = SpringMassModel()
    model.set_damping(7.0)
    equilibrium = model.get_equilibrium_extension()
    model.displacement = equilibrium + 0.6
    for _ in range(900):
        model.step(1.0 / 60.0)
    assert abs(model.displacement - equilibrium) < 0.08
    assert abs(model.velocity) < 0.35


if __name__ == "__main__":
    test_initializes()
    test_mass_increases_equilibrium_extension()
    test_spring_constant_decreases_equilibrium_extension()
    test_damping_reduces_velocity()
    test_reset_restores_initial_state()
    test_dragging_changes_displacement()
    test_release_resumes_physics()
    test_no_damping_stays_bounded_short_term()
    test_damping_settles_toward_equilibrium()
    print("SpringMassModel tests passed")

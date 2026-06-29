from collision_model import CollisionLabModel


TOL = 1e-6


def close(a, b, tol=TOL):
    return abs(a - b) <= tol


def resolve_collision(model):
    model.playing = True
    model.step(1 / 60)


def test_equal_masses_swap_velocities():
    model = CollisionLabModel()
    a, b = model.balls
    a.mass = b.mass = 1
    a.radius = b.radius = 20
    a.x = 200
    b.x = 239
    a.velocity_x = 50
    b.velocity_x = -30

    resolve_collision(model)
    assert close(a.velocity_x, -30)
    assert close(b.velocity_x, 50)


def test_heavy_ball_affects_light_ball_correctly():
    model = CollisionLabModel()
    a, b = model.balls
    a.mass = 5
    b.mass = 1
    a.radius = b.radius = 20
    a.x = 200
    b.x = 239
    a.velocity_x = 20
    b.velocity_x = -20

    resolve_collision(model)
    expected_a = ((5 - 1) / 6) * 20 + ((2 * 1) / 6) * -20
    expected_b = ((2 * 5) / 6) * 20 + ((1 - 5) / 6) * -20
    assert close(a.velocity_x, expected_a)
    assert close(b.velocity_x, expected_b)


def test_total_momentum_conserved():
    model = CollisionLabModel()
    a, b = model.balls
    a.mass = 3
    b.mass = 2
    a.radius = b.radius = 20
    a.x = 200
    b.x = 239
    a.velocity_x = 60
    b.velocity_x = -25

    before = model.total_momentum()
    resolve_collision(model)
    after = model.total_momentum()
    assert close(before, after)


def test_total_kinetic_energy_conserved_for_elastic_collision():
    model = CollisionLabModel()
    a, b = model.balls
    a.mass = 3
    b.mass = 2
    a.radius = b.radius = 20
    a.x = 200
    b.x = 239
    a.velocity_x = 60
    b.velocity_x = -25

    before = model.total_kinetic_energy()
    resolve_collision(model)
    after = model.total_kinetic_energy()
    assert close(before, after)


def test_wall_bounce_flips_velocity():
    model = CollisionLabModel()
    ball = model.balls[0]
    ball.x = model.left_wall + ball.radius - 1
    ball.velocity_x = -40
    model.playing = True
    model.step(1 / 60)
    assert ball.velocity_x == 40


def test_reset_restores_initial_state():
    model = CollisionLabModel()
    model.balls[0].x = 250
    model.balls[0].mass = 4
    model.balls[0].velocity_x = -100
    model.playing = True

    model.reset()
    a, b = model.balls
    assert not model.playing
    assert a.x == 145.0
    assert a.mass == 1.0
    assert a.velocity_x == 70.0
    assert b.x == 335.0
    assert b.mass == 2.0
    assert b.velocity_x == -45.0


if __name__ == "__main__":
    test_equal_masses_swap_velocities()
    test_heavy_ball_affects_light_ball_correctly()
    test_total_momentum_conserved()
    test_total_kinetic_energy_conserved_for_elastic_collision()
    test_wall_bounce_flips_velocity()
    test_reset_restores_initial_state()
    print("CollisionLabModel Python tests passed")

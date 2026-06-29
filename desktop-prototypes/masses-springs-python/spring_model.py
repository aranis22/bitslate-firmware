"""Pure-ish spring/mass model for the BitSlate desktop prototype."""


class SpringMassModel:
    PIXELS_PER_METER = 140.0

    def __init__(self):
        self.anchor_x = 160
        self.anchor_y = 34
        self.rest_length = 88
        self.default_mass_kg = 1.0
        self.default_spring_k = 80.0
        self.default_damping = 1.5
        self.default_gravity = 9.8
        self.reset()

    def reset(self):
        self.mass_kg = self.default_mass_kg
        self.spring_k = self.default_spring_k
        self.damping = self.default_damping
        self.gravity = self.default_gravity
        self.displacement = self.get_equilibrium_extension()
        self.velocity = 0.0
        self.playing = True
        self.dragging = False

    def set_mass(self, value):
        old_equilibrium = self.get_equilibrium_extension()
        self.mass_kg = self._clamp(float(value), 0.5, 5.0)
        self.displacement += self.get_equilibrium_extension() - old_equilibrium

    def set_spring_k(self, value):
        old_equilibrium = self.get_equilibrium_extension()
        self.spring_k = self._clamp(float(value), 20.0, 200.0)
        self.displacement += self.get_equilibrium_extension() - old_equilibrium

    def set_damping(self, value):
        self.damping = self._clamp(float(value), 0.0, 8.0)

    def set_gravity(self, value):
        old_equilibrium = self.get_equilibrium_extension()
        self.gravity = self._clamp(float(value), 0.0, 9.8)
        self.displacement += self.get_equilibrium_extension() - old_equilibrium

    def start_drag(self, screen_y):
        self.dragging = True
        self.playing = False
        self.drag_to(screen_y)

    def drag_to(self, screen_y):
        if not self.dragging:
            return
        raw = (float(screen_y) - self.anchor_y - self.rest_length) / self.PIXELS_PER_METER
        self.displacement = self._clamp(raw, -0.35, 1.25)
        self.velocity = 0.0

    def release_drag(self):
        self.dragging = False
        self.playing = True

    def step(self, dt):
        if not self.playing or self.dragging:
            return

        dt = self._clamp(float(dt), 0.0, 1.0 / 20.0)
        acceleration = self.get_net_force() / self.mass_kg
        self.velocity += acceleration * dt
        self.velocity = self._clamp(self.velocity, -10.0, 10.0)
        self.displacement += self.velocity * dt
        self.displacement = self._clamp(self.displacement, -0.35, 1.25)

        if self.displacement <= -0.35 and self.velocity < 0.0:
            self.velocity = 0.0
        if self.displacement >= 1.25 and self.velocity > 0.0:
            self.velocity = 0.0

    def get_mass_y(self):
        return self.anchor_y + self.rest_length + self.displacement * self.PIXELS_PER_METER

    def get_equilibrium_extension(self):
        return (self.mass_kg * self.gravity) / self.spring_k if self.spring_k > 0.0 else 0.0

    def get_spring_force(self):
        return -self.spring_k * self.displacement

    def get_gravity_force(self):
        return self.mass_kg * self.gravity

    def get_damping_force(self):
        return -self.damping * self.velocity

    def get_net_force(self):
        return self.get_spring_force() + self.get_gravity_force() + self.get_damping_force()

    @staticmethod
    def _clamp(value, low, high):
        return max(low, min(high, value))

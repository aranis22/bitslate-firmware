from __future__ import annotations

from dataclasses import dataclass


DEFAULT_WIDTH = 480
DEFAULT_HEIGHT = 320


@dataclass
class Ball:
    id: int
    x: float
    y: float
    radius: float
    mass: float
    velocity_x: float
    color: tuple[int, int, int] = (80, 120, 220)
    name: str = "Ball"


class CollisionLabModel:
    def __init__(self, width: int = DEFAULT_WIDTH, height: int = DEFAULT_HEIGHT) -> None:
        self.width = width
        self.height = height
        self.left_wall = 28.0
        self.right_wall = float(width - 28)
        self.track_y = 122.0
        self.playing = False
        self.status = "paused"
        self.balls: list[Ball] = []
        self.reset()

    def reset(self) -> None:
        self.playing = False
        self.status = "paused"
        self.balls = [
            Ball(1, 145.0, self.track_y, 18.0, 1.0, 70.0, (75, 135, 245), "A"),
            Ball(2, 335.0, self.track_y, 24.0, 2.0, -45.0, (236, 109, 75), "B"),
        ]

    def step(self, dt: float) -> None:
        if not self.playing:
            return

        # Clamp large frame gaps so the simple v0 collision code stays stable.
        dt = max(0.0, min(dt, 1.0 / 20.0))
        for ball in self.balls:
            ball.x += ball.velocity_x * dt

        self._resolve_wall_collisions()
        self._resolve_ball_collision()

    def set_mass(self, ball_id: int, mass: float) -> None:
        ball = self.get_ball(ball_id)
        if ball:
            ball.mass = max(1.0, min(5.0, mass))
            ball.radius = 14.0 + ball.mass * 4.0
            self._separate_balls()
            self._keep_balls_inside()

    def set_velocity(self, ball_id: int, velocity_x: float) -> None:
        ball = self.get_ball(ball_id)
        if ball:
            ball.velocity_x = max(-120.0, min(120.0, velocity_x))

    def get_ball(self, ball_id: int) -> Ball | None:
        return next((ball for ball in self.balls if ball.id == ball_id), None)

    def total_momentum(self) -> float:
        return sum(ball.mass * ball.velocity_x for ball in self.balls)

    def total_kinetic_energy(self) -> float:
        return sum(0.5 * ball.mass * ball.velocity_x * ball.velocity_x for ball in self.balls)

    def _resolve_wall_collisions(self) -> None:
        for ball in self.balls:
            if ball.x - ball.radius < self.left_wall:
                ball.x = self.left_wall + ball.radius
                if ball.velocity_x < 0:
                    ball.velocity_x = -ball.velocity_x
                    self.status = "wall bounce"
            elif ball.x + ball.radius > self.right_wall:
                ball.x = self.right_wall - ball.radius
                if ball.velocity_x > 0:
                    ball.velocity_x = -ball.velocity_x
                    self.status = "wall bounce"

    def _resolve_ball_collision(self) -> None:
        if len(self.balls) < 2:
            return

        left, right = sorted(self.balls, key=lambda ball: ball.x)
        distance = right.x - left.x
        min_distance = left.radius + right.radius
        relative_velocity = left.velocity_x - right.velocity_x

        if distance <= min_distance and relative_velocity > 0:
            v1 = left.velocity_x
            v2 = right.velocity_x
            m1 = left.mass
            m2 = right.mass

            left.velocity_x = ((m1 - m2) / (m1 + m2)) * v1 + ((2.0 * m2) / (m1 + m2)) * v2
            right.velocity_x = ((2.0 * m1) / (m1 + m2)) * v1 + ((m2 - m1) / (m1 + m2)) * v2
            self._separate_pair(left, right)
            self.status = "collision"

    def _separate_balls(self) -> None:
        if len(self.balls) < 2:
            return
        left, right = sorted(self.balls, key=lambda ball: ball.x)
        self._separate_pair(left, right)

    def _separate_pair(self, left: Ball, right: Ball) -> None:
        min_distance = left.radius + right.radius
        overlap = min_distance - (right.x - left.x)
        if overlap <= 0:
            return

        left.x -= overlap / 2.0 + 0.1
        right.x += overlap / 2.0 + 0.1
        self._keep_balls_inside()

    def _keep_balls_inside(self) -> None:
        for ball in self.balls:
            ball.x = max(self.left_wall + ball.radius, min(self.right_wall - ball.radius, ball.x))

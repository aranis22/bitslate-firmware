from __future__ import annotations

import math
import sys

import pygame

from collision_model import CollisionLabModel


WIDTH = 480
HEIGHT = 320
CONTROL_TOP = 228
FPS = 60

BG = (232, 240, 252)
TRACK_BG = (214, 226, 247)
PANEL = (245, 246, 248)
TEXT = (20, 26, 36)
MUTED = (88, 98, 112)
BUTTON = (224, 232, 244)
BUTTON_ACTIVE = (199, 224, 255)
SLIDER_TRACK = (150, 160, 176)
SLIDER_KNOB = (48, 97, 176)
WALL = (40, 48, 60)
ARROW = (30, 90, 185)


class Slider:
    def __init__(self, label: str, rect: pygame.Rect, min_value: float, max_value: float, value: float, ball_id: int, kind: str) -> None:
        self.label = label
        self.rect = rect
        self.min_value = min_value
        self.max_value = max_value
        self.value = value
        self.ball_id = ball_id
        self.kind = kind
        self.dragging = False

    def set_from_x(self, x: int) -> None:
        t = (x - self.rect.left) / self.rect.width
        t = max(0.0, min(1.0, t))
        self.value = self.min_value + t * (self.max_value - self.min_value)

    def knob_x(self) -> int:
        t = (self.value - self.min_value) / (self.max_value - self.min_value)
        return int(self.rect.left + t * self.rect.width)


class CollisionLabApp:
    def __init__(self) -> None:
        pygame.init()
        pygame.display.set_caption("BitSlate Collision Lab Prototype")
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("arial", 15)
        self.small_font = pygame.font.SysFont("arial", 12)
        self.model = CollisionLabModel(WIDTH, HEIGHT)
        self.play_button = pygame.Rect(12, 238, 74, 28)
        self.reset_button = pygame.Rect(12, 276, 74, 28)
        self.sliders = [
            Slider("Mass A", pygame.Rect(120, 238, 96, 10), 1, 5, self.model.balls[0].mass, 1, "mass"),
            Slider("Vel A", pygame.Rect(120, 276, 96, 10), -120, 120, self.model.balls[0].velocity_x, 1, "velocity"),
            Slider("Mass B", pygame.Rect(294, 238, 96, 10), 1, 5, self.model.balls[1].mass, 2, "mass"),
            Slider("Vel B", pygame.Rect(294, 276, 96, 10), -120, 120, self.model.balls[1].velocity_x, 2, "velocity"),
        ]
        self.dragging_slider: Slider | None = None

    def run(self) -> None:
        while True:
            dt = self.clock.tick(FPS) / 1000.0
            self.handle_events()
            self.model.step(dt)
            self.sync_sliders_from_model()
            self.draw()
            pygame.display.flip()

    def handle_events(self) -> None:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.quit()
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                self.quit()
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                self.on_mouse_down(event.pos)
            elif event.type == pygame.MOUSEMOTION:
                self.on_mouse_motion(event.pos)
            elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
                self.dragging_slider = None

    def on_mouse_down(self, pos: tuple[int, int]) -> None:
        if self.play_button.collidepoint(pos):
            self.model.playing = not self.model.playing
            self.model.status = "playing" if self.model.playing else "paused"
            return

        if self.reset_button.collidepoint(pos):
            self.model.reset()
            self.sync_sliders_from_model()
            return

        for slider in self.sliders:
            hit_rect = slider.rect.inflate(18, 18)
            if hit_rect.collidepoint(pos):
                self.model.playing = False
                slider.dragging = True
                self.dragging_slider = slider
                self.update_slider(slider, pos[0])
                return

    def on_mouse_motion(self, pos: tuple[int, int]) -> None:
        if self.dragging_slider:
            self.update_slider(self.dragging_slider, pos[0])

    def update_slider(self, slider: Slider, x: int) -> None:
        slider.set_from_x(x)
        if slider.kind == "mass":
            self.model.set_mass(slider.ball_id, slider.value)
        else:
            self.model.set_velocity(slider.ball_id, slider.value)
        self.model.status = "paused"

    def sync_sliders_from_model(self) -> None:
        for slider in self.sliders:
            ball = self.model.get_ball(slider.ball_id)
            if not ball or slider.dragging:
                continue
            slider.value = ball.mass if slider.kind == "mass" else ball.velocity_x

    def draw(self) -> None:
        self.screen.fill(BG)
        self.draw_track()
        self.draw_balls()
        self.draw_readouts()
        self.draw_controls()

    def draw_track(self) -> None:
        pygame.draw.rect(self.screen, TRACK_BG, (0, 0, WIDTH, CONTROL_TOP))
        pygame.draw.line(self.screen, WALL, (self.model.left_wall, 48), (self.model.left_wall, 190), 4)
        pygame.draw.line(self.screen, WALL, (self.model.right_wall, 48), (self.model.right_wall, 190), 4)
        pygame.draw.line(self.screen, (130, 144, 166), (self.model.left_wall, self.model.track_y), (self.model.right_wall, self.model.track_y), 2)
        self.draw_text("Collision Lab v0", 12, 10, self.font, TEXT)

    def draw_balls(self) -> None:
        for ball in self.model.balls:
            pygame.draw.circle(self.screen, ball.color, (int(ball.x), int(ball.y)), int(ball.radius))
            pygame.draw.circle(self.screen, (255, 255, 255), (int(ball.x), int(ball.y)), int(ball.radius), 2)
            self.draw_centered_text(ball.name, int(ball.x), int(ball.y) - 7, self.font, (255, 255, 255))
            self.draw_velocity_arrow(ball)

    def draw_velocity_arrow(self, ball) -> None:
        length = max(-55, min(55, ball.velocity_x * 0.35))
        if abs(length) < 4:
            return
        start = (int(ball.x), int(ball.y - ball.radius - 18))
        end = (int(ball.x + length), start[1])
        pygame.draw.line(self.screen, ARROW, start, end, 3)
        direction = 1 if length > 0 else -1
        pygame.draw.polygon(
            self.screen,
            ARROW,
            [
                end,
                (end[0] - direction * 8, end[1] - 5),
                (end[0] - direction * 8, end[1] + 5),
            ],
        )

    def draw_readouts(self) -> None:
        status = "playing" if self.model.playing else self.model.status
        self.draw_text(f"Status: {status}", 12, 198, self.small_font, TEXT)
        self.draw_text(f"Momentum: {self.model.total_momentum():.1f}", 160, 198, self.small_font, TEXT)
        self.draw_text(f"Energy: {self.model.total_kinetic_energy():.1f}", 305, 198, self.small_font, TEXT)

    def draw_controls(self) -> None:
        pygame.draw.rect(self.screen, PANEL, (0, CONTROL_TOP, WIDTH, HEIGHT - CONTROL_TOP))
        pygame.draw.line(self.screen, (190, 195, 205), (0, CONTROL_TOP), (WIDTH, CONTROL_TOP), 1)
        self.draw_button(self.play_button, "Pause" if self.model.playing else "Play", self.model.playing)
        self.draw_button(self.reset_button, "Reset", False)
        for slider in self.sliders:
            self.draw_slider(slider)

    def draw_button(self, rect: pygame.Rect, label: str, active: bool) -> None:
        pygame.draw.rect(self.screen, BUTTON_ACTIVE if active else BUTTON, rect, border_radius=5)
        pygame.draw.rect(self.screen, (94, 108, 132), rect, 1, border_radius=5)
        self.draw_centered_text(label, rect.centerx, rect.centery - 7, self.font, TEXT)

    def draw_slider(self, slider: Slider) -> None:
        x_label = slider.rect.left
        y_label = slider.rect.top - 18
        self.draw_text(f"{slider.label}: {slider.value:.1f}", x_label, y_label, self.small_font, TEXT)
        pygame.draw.line(self.screen, SLIDER_TRACK, slider.rect.midleft, slider.rect.midright, 4)
        pygame.draw.circle(self.screen, SLIDER_KNOB, (slider.knob_x(), slider.rect.centery), 8)

    def draw_text(self, text: str, x: int, y: int, font: pygame.font.Font, color: tuple[int, int, int]) -> None:
        self.screen.blit(font.render(text, True, color), (x, y))

    def draw_centered_text(self, text: str, x: int, y: int, font: pygame.font.Font, color: tuple[int, int, int]) -> None:
        surface = font.render(text, True, color)
        self.screen.blit(surface, surface.get_rect(center=(x, y)))

    def quit(self) -> None:
        pygame.quit()
        sys.exit(0)


if __name__ == "__main__":
    CollisionLabApp().run()

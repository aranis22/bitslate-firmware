import math
import sys

import pygame

from spring_model import SpringMassModel


WIDTH = 480
HEIGHT = 320
FPS = 60
SPRING_AREA_Y_OFFSET = 22

BG = (239, 244, 252)
PANEL = (222, 232, 246)
INK = (24, 34, 50)
MUTED = (96, 112, 132)
BLUE = (54, 105, 220)
BLUE_DARK = (31, 73, 170)
ORANGE = (220, 123, 54)
GREEN = (44, 150, 90)
RED = (210, 64, 70)
SPRING = (70, 78, 92)


class Button:
    def __init__(self, rect, label, callback):
        self.rect = pygame.Rect(rect)
        self.label = label
        self.callback = callback

    def draw(self, screen, font, active=False):
        fill = BLUE if active else (232, 238, 248)
        border = BLUE_DARK if active else (164, 178, 198)
        text_color = (255, 255, 255) if active else INK
        pygame.draw.rect(screen, fill, self.rect, border_radius=5)
        pygame.draw.rect(screen, border, self.rect, 1, border_radius=5)
        text = font.render(self.label, True, text_color)
        screen.blit(text, text.get_rect(center=self.rect.center))

    def handle_down(self, pos):
        if self.rect.collidepoint(pos):
            self.callback()
            return True
        return False


class Slider:
    def __init__(self, rect, minimum, maximum, value, setter):
        self.rect = pygame.Rect(rect)
        self.minimum = minimum
        self.maximum = maximum
        self.value = value
        self.setter = setter
        self.dragging = False

    def set_value(self, value):
        self.value = max(self.minimum, min(self.maximum, value))
        self.setter(self.value)

    def value_from_x(self, x):
        ratio = (x - self.rect.left) / self.rect.width
        return self.minimum + max(0.0, min(1.0, ratio)) * (self.maximum - self.minimum)

    def handle_down(self, pos):
        knob = self.knob_rect()
        hit = self.rect.inflate(10, 20)
        if knob.collidepoint(pos) or hit.collidepoint(pos):
            self.dragging = True
            self.set_value(self.value_from_x(pos[0]))
            return True
        return False

    def handle_motion(self, pos):
        if self.dragging:
            self.set_value(self.value_from_x(pos[0]))

    def handle_up(self):
        self.dragging = False

    def knob_rect(self):
        ratio = (self.value - self.minimum) / (self.maximum - self.minimum)
        x = self.rect.left + ratio * self.rect.width
        return pygame.Rect(int(x - 6), self.rect.centery - 10, 12, 20)

    def draw(self, screen):
        pygame.draw.line(screen, (168, 185, 210), self.rect.midleft, self.rect.midright, 4)
        pygame.draw.line(screen, BLUE, self.rect.midleft, self.knob_rect().center, 4)
        pygame.draw.ellipse(screen, BLUE, self.knob_rect())


def draw_text(screen, font, text, x, y, color=INK):
    screen.blit(font.render(text, True, color), (x, y))


def visual_y(model_y):
    return int(model_y + SPRING_AREA_Y_OFFSET)


def draw_spring(screen, model):
    top = (model.anchor_x, visual_y(model.anchor_y))
    bottom_y = visual_y(model.get_mass_y() - 22)
    coils = 9
    width = 26
    points = [top, (model.anchor_x, visual_y(model.anchor_y + 12))]
    span = max(20, bottom_y - visual_y(model.anchor_y) - 24)
    for i in range(coils + 1):
        y = visual_y(model.anchor_y + 12) + span * i / coils
        x = model.anchor_x + (-width // 2 if i % 2 == 0 else width // 2)
        points.append((int(x), int(y)))
    points.append((model.anchor_x, bottom_y))
    pygame.draw.lines(screen, SPRING, False, points, 3)
    pygame.draw.circle(screen, INK, top, 5)
    pygame.draw.line(screen, INK, (model.anchor_x - 55, visual_y(model.anchor_y - 8)), (model.anchor_x + 55, visual_y(model.anchor_y - 8)), 4)


def draw_dashed_line(screen, y, color, start_x=54, end_x=280):
    x = start_x
    while x < end_x:
        pygame.draw.line(screen, color, (x, int(y)), (min(x + 8, end_x), int(y)), 1)
        x += 14


def draw_model(screen, model, big_font, font, small_font):
    pygame.draw.rect(screen, (246, 249, 255), (8, 8, 304, 304), border_radius=8)
    pygame.draw.rect(screen, (180, 195, 218), (8, 8, 304, 304), 1, border_radius=8)
    draw_text(screen, big_font, "Masses & Springs", 22, 16)

    rest_y = visual_y(model.anchor_y + model.rest_length)
    equilibrium_y = visual_y(model.anchor_y + model.rest_length + model.get_equilibrium_extension() * model.PIXELS_PER_METER)
    draw_dashed_line(screen, rest_y, MUTED)
    draw_text(screen, small_font, "rest", 60, int(rest_y - 15), MUTED)
    draw_dashed_line(screen, equilibrium_y, GREEN)
    draw_text(screen, small_font, "equilibrium", 60, int(equilibrium_y + 4), GREEN)

    draw_spring(screen, model)

    mass_y = visual_y(model.get_mass_y())
    mass_rect = pygame.Rect(model.anchor_x - 30, mass_y - 18, 60, 36)
    pygame.draw.rect(screen, ORANGE, mass_rect, border_radius=7)
    pygame.draw.rect(screen, (130, 74, 35), mass_rect, 2, border_radius=7)
    label = font.render(f"{model.mass_kg:.1f} kg", True, (255, 255, 255))
    screen.blit(label, label.get_rect(center=mass_rect.center))

    pygame.draw.line(screen, (126, 146, 172), (model.anchor_x, mass_y - 18), (model.anchor_x, mass_y - 34), 2)

    status = "Dragging" if model.dragging else ("Playing" if model.playing else "Paused")
    if model.playing and abs(model.velocity) < 0.08 and abs(model.displacement - model.get_equilibrium_extension()) < 0.02:
        status = "Settling"
    draw_text(screen, font, f"Status: {status}", 24, 244)
    draw_text(screen, small_font, f"Displacement: {model.displacement:.2f} m", 24, 266)
    draw_text(screen, small_font, f"Spring Force: {model.get_spring_force():.1f} N", 24, 284)


def draw_controls(screen, model, buttons, sliders, font, small_font):
    pygame.draw.rect(screen, PANEL, (320, 8, 152, 304), border_radius=8)
    pygame.draw.rect(screen, (172, 188, 210), (320, 8, 152, 304), 1, border_radius=8)

    buttons[0].label = "Pause" if model.playing else "Play"
    buttons[0].draw(screen, font, active=model.playing)
    buttons[1].draw(screen, font, active=False)

    rows = [
        ("Mass", f"{model.mass_kg:.1f} kg", sliders[0]),
        ("k", f"{model.spring_k:.0f} N/m", sliders[1]),
        ("Damping", f"{model.damping:.1f}", sliders[2]),
        ("Gravity", f"{model.gravity:.1f} m/s^2", sliders[3])
    ]

    y = 92
    for name, value, slider in rows:
        draw_text(screen, small_font, name, 334, y)
        draw_text(screen, small_font, value, 410, y, MUTED)
        slider.draw(screen)
        y += 46

    draw_text(screen, small_font, f"Gravity Force: {model.get_gravity_force():.1f} N", 334, 276, MUTED)
    draw_text(screen, small_font, f"Net Force: {model.get_net_force():.1f} N", 334, 294, MUTED)


def mass_hit(model, pos):
    mass_y = visual_y(model.get_mass_y())
    return pygame.Rect(model.anchor_x - 34, mass_y - 24, 68, 48).collidepoint(pos)


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("BitSlate Masses & Springs Prototype")
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("arial", 16)
    small_font = pygame.font.SysFont("arial", 14)
    big_font = pygame.font.SysFont("arial", 24)

    model = SpringMassModel()

    def toggle_play():
        if not model.dragging:
            model.playing = not model.playing

    def reset():
        model.reset()
        sliders[0].value = model.mass_kg
        sliders[1].value = model.spring_k
        sliders[2].value = model.damping
        sliders[3].value = model.gravity

    buttons = [
        Button((334, 32, 58, 30), "Pause", toggle_play),
        Button((402, 32, 58, 30), "Reset", reset)
    ]
    sliders = [
        Slider((334, 120, 122, 10), 0.5, 5.0, model.mass_kg, model.set_mass),
        Slider((334, 166, 122, 10), 20.0, 200.0, model.spring_k, model.set_spring_k),
        Slider((334, 212, 122, 10), 0.0, 8.0, model.damping, model.set_damping),
        Slider((334, 258, 122, 10), 0.0, 9.8, model.gravity, model.set_gravity)
    ]

    running = True
    while running:
        dt = clock.tick(FPS) / 1000.0
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                pos = event.pos
                handled = any(button.handle_down(pos) for button in buttons)
                if not handled:
                    handled = any(slider.handle_down(pos) for slider in sliders)
                if not handled and mass_hit(model, pos):
                    model.start_drag(pos[1] - SPRING_AREA_Y_OFFSET)
            elif event.type == pygame.MOUSEMOTION:
                for slider in sliders:
                    slider.handle_motion(event.pos)
                if model.dragging:
                    model.drag_to(event.pos[1] - SPRING_AREA_Y_OFFSET)
            elif event.type == pygame.MOUSEBUTTONUP and event.button == 1:
                for slider in sliders:
                    slider.handle_up()
                if model.dragging:
                    model.release_drag()

        model.step(dt)
        screen.fill(BG)
        draw_model(screen, model, big_font, font, small_font)
        draw_controls(screen, model, buttons, sliders, font, small_font)
        pygame.display.flip()

    pygame.quit()
    return 0


if __name__ == "__main__":
    sys.exit(main())

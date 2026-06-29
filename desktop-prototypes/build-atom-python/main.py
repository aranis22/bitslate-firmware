import math

import pygame

from atom_model import AtomModel


WIDTH = 480
HEIGHT = 320
SIDEBAR_WIDTH = 104

COLORS = {
    "background": (222, 235, 255),
    "panel": (246, 248, 252),
    "panel_line": (158, 169, 184),
    "text": (28, 35, 48),
    "muted": (82, 93, 112),
    "button": (236, 240, 247),
    "button_outline": (145, 155, 170),
    "proton": (224, 74, 74),
    "neutron": (118, 126, 136),
    "electron": (61, 116, 226),
    "nucleus": (236, 196, 113),
    "nucleus_outline": (118, 86, 56),
    "shell": (154, 178, 228),
}

TRAY_BUTTONS = [
    ("Proton", pygame.Rect(10, 24, 84, 58)),
    ("Neutron", pygame.Rect(10, 92, 84, 58)),
    ("Electron", pygame.Rect(10, 160, 84, 58)),
    ("Reset", pygame.Rect(10, 250, 84, 44)),
]


def draw_text(surface, font, text, x, y, color=(30, 36, 48)):
    surface.blit(font.render(text, True, color), (x, y))


def draw_centered_text(surface, font, text, rect, color=COLORS["text"]):
    image = font.render(text, True, color)
    surface.blit(image, image.get_rect(center=rect.center))


def draw_tray_particle_icon(screen, label, rect):
    color_by_label = {
        "Proton": COLORS["proton"],
        "Neutron": COLORS["neutron"],
        "Electron": COLORS["electron"],
    }
    color = color_by_label.get(label)
    if color is None:
        return

    center = (rect.centerx, rect.y + 41)
    pygame.draw.circle(screen, color, center, 8)
    pygame.draw.circle(screen, (255, 255, 255), (center[0] - 3, center[1] - 3), 2)
    pygame.draw.circle(screen, COLORS["button_outline"], center, 8, 1)


def draw_tray(screen, font, small_font, atom):
    pygame.draw.rect(screen, COLORS["panel"], (0, 0, SIDEBAR_WIDTH, HEIGHT))
    pygame.draw.line(screen, COLORS["panel_line"], (SIDEBAR_WIDTH, 0), (SIDEBAR_WIDTH, HEIGHT), 2)

    counts = {
        "Proton": atom.protons,
        "Neutron": atom.neutrons,
        "Electron": atom.electrons,
    }

    for label, rect in TRAY_BUTTONS:
        pygame.draw.rect(screen, COLORS["button"], rect, border_radius=6)
        pygame.draw.rect(screen, COLORS["button_outline"], rect, 1, border_radius=6)
        if label in counts:
            text_image = font.render(label, True, COLORS["text"])
            screen.blit(text_image, text_image.get_rect(center=(rect.centerx, rect.y + 18)))
            draw_tray_particle_icon(screen, label, rect)
        else:
            draw_centered_text(screen, font, label, rect)
        if label in counts:
            draw_text(screen, small_font, str(counts[label]), rect.right - 18, rect.bottom - 18, COLORS["muted"])


def draw_particle_cluster(screen, center, protons, neutrons):
    positions = [
        (0, 0),
        (-13, -8),
        (13, -8),
        (-13, 10),
        (13, 10),
        (0, -20),
        (0, 20),
        (-24, 0),
        (24, 0),
        (-22, -18),
        (22, 18),
        (22, -18),
        (-22, 18),
        (0, -34),
        (0, 34),
        (-34, 0),
        (34, 0),
        (-32, -28),
        (32, -28),
        (-32, 28),
        (32, 28),
    ]

    particles = [COLORS["proton"]] * protons + [COLORS["neutron"]] * neutrons
    for index, color in enumerate(particles[: len(positions)]):
        dx, dy = positions[index]
        pygame.draw.circle(screen, color, (center[0] + dx, center[1] + dy), 8)
        pygame.draw.circle(screen, (255, 255, 255), (center[0] + dx - 3, center[1] + dy - 3), 2)


def draw_electrons(screen, center, atom):
    shell_one, shell_two = atom.electron_shell_counts()
    shells = [(48, shell_one), (78, shell_two)]
    for radius, count in shells:
        if count == 0:
            continue
        for index in range(count):
            angle = (index / max(count, 1)) * math.tau - (math.pi / 2)
            x = int(center[0] + radius * math.cos(angle))
            y = int(center[1] + radius * math.sin(angle))
            pygame.draw.circle(screen, COLORS["electron"], (x, y), 6)
            pygame.draw.circle(screen, (245, 250, 255), (x - 2, y - 2), 2)


def draw_atom_workspace(screen, font, small_font, atom):
    draw_text(screen, font, "Build an Atom", 124, 16)
    center = (268, 158)

    pygame.draw.circle(screen, COLORS["shell"], center, 78, 2)
    pygame.draw.circle(screen, COLORS["shell"], center, 48, 2)
    pygame.draw.circle(screen, COLORS["nucleus"], center, 35)
    pygame.draw.circle(screen, COLORS["nucleus_outline"], center, 35, 2)
    draw_particle_cluster(screen, center, atom.protons, atom.neutrons)
    draw_electrons(screen, center, atom)

    draw_text(screen, small_font, "shell 2", center[0] - 20, center[1] - 99, COLORS["muted"])
    draw_text(screen, small_font, "shell 1", center[0] - 20, center[1] - 69, COLORS["muted"])

    info_x = 364
    draw_text(screen, font, atom.element_symbol(), info_x, 58)
    draw_text(screen, small_font, atom.element_name(), info_x, 88)
    draw_text(screen, small_font, f"Atomic #: {atom.atomic_number()}", info_x, 124)
    draw_text(screen, small_font, f"Mass #: {atom.mass_number()}", info_x, 150)
    draw_text(screen, small_font, f"Charge: {atom.charge()}", info_x, 176)

    draw_text(screen, small_font, "Click tray buttons to add particles.", 126, 284, COLORS["muted"])


def handle_click(pos, atom):
    for label, rect in TRAY_BUTTONS:
        if rect.collidepoint(pos):
            if label == "Proton":
                atom.add_proton()
            elif label == "Neutron":
                atom.add_neutron()
            elif label == "Electron":
                atom.add_electron()
            elif label == "Reset":
                atom.reset()
            return


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("BitSlate Build an Atom")
    clock = pygame.time.Clock()
    font = pygame.font.SysFont("arial", 20)
    small_font = pygame.font.SysFont("arial", 16)
    atom = AtomModel()

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                handle_click(event.pos, atom)
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_p:
                    atom.add_proton()
                elif event.key == pygame.K_n:
                    atom.add_neutron()
                elif event.key == pygame.K_e:
                    atom.add_electron()
                elif event.key == pygame.K_r:
                    atom.reset()

        screen.fill(COLORS["background"])
        draw_tray(screen, font, small_font, atom)
        draw_atom_workspace(screen, font, small_font, atom)

        pygame.display.flip()
        clock.tick(30)

    pygame.quit()


if __name__ == "__main__":
    main()

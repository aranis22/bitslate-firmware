import pygame

from elements_data import UNKNOWN
from periodic_model import PeriodicTableModel


WIDTH = 480
HEIGHT = 320
PANEL_W = 125
GRID_X = PANEL_W + 8
GRID_Y = 42
CELL_W = 18
CELL_H = 24
GAP = 1

CATEGORY_COLORS = {
    "Alkali metal": (222, 202, 99),
    "Alkaline earth metal": (230, 236, 105),
    "Transition metal": (231, 190, 176),
    "Post-transition metal": (177, 215, 225),
    "Metalloid": (166, 218, 204),
    "Nonmetal": (165, 226, 145),
    "Halogen": (172, 234, 145),
    "Noble gas": (210, 178, 217),
    "Lanthanide": (224, 206, 151),
    "Actinide": (225, 193, 207),
    "Unknown": (220, 220, 220),
    UNKNOWN: (220, 220, 220),
}


def draw_text(surface, font, text, x, y, color=(24, 34, 52)):
    surface.blit(font.render(str(text), True, color), (x, y))


def fit_text(surface, font, text, rect, color=(24, 34, 52)):
    label = str(text)
    while len(label) > 3 and font.size(label)[0] > rect.width:
        label = label[:-1]
    surface.blit(font.render(label, True, color), (rect.x, rect.y))


def cell_rect(element):
    x = GRID_X + (element["grid_col"] - 1) * (CELL_W + GAP)
    y = GRID_Y + (element["grid_row"] - 1) * (CELL_H + GAP)
    return pygame.Rect(x, y, CELL_W, CELL_H)


def draw_info_panel(screen, model, fonts):
    element = model.selected_element()
    pygame.draw.rect(screen, (242, 246, 252), (0, 0, PANEL_W, HEIGHT))
    pygame.draw.line(screen, (150, 165, 185), (PANEL_W, 0), (PANEL_W, HEIGHT), 2)

    draw_text(screen, fonts["small"], "Selected", 10, 10, (78, 88, 104))
    draw_text(screen, fonts["symbol"], element["symbol"], 10, 32)
    draw_text(screen, fonts["name"], element["name"], 10, 78)
    pygame.draw.line(screen, (194, 205, 220), (10, 102), (PANEL_W - 10, 102), 1)

    rows = [
        ("Atomic #", element["atomic_number"]),
        ("Mass", element["atomic_mass"]),
        ("Category", element["category"]),
        ("Group/Period", f"{model.format_value(element['group'])} / {model.format_value(element['period'])}"),
        ("Shells", element["shells"]),
        ("State", element["state"]),
        ("Electroneg.", element["electronegativity"]),
    ]
    y = 110
    for label, value in rows:
        draw_text(screen, fonts["tiny"], label, 10, y, (82, 92, 108))
        fit_text(screen, fonts["tiny"], model.format_value(value), pygame.Rect(10, y + 11, PANEL_W - 18, 12))
        y += 29


def draw_table(screen, model, fonts):
    draw_text(screen, fonts["title"], "Periodic Table", GRID_X, 10)
    selected = model.selected_element()

    for element in model.all_elements():
        rect = cell_rect(element)
        fill = CATEGORY_COLORS.get(element["category"], CATEGORY_COLORS["Unknown"])
        pygame.draw.rect(screen, fill, rect)
        border = (20, 66, 150) if element == selected else (145, 155, 170)
        pygame.draw.rect(screen, border, rect, 2 if element == selected else 1)
        draw_text(screen, fonts["micro"], element["atomic_number"], rect.x + 1, rect.y + 1, (35, 45, 58))
        symbol_img = fonts["cell"].render(element["symbol"], True, (6, 22, 42))
        screen.blit(symbol_img, symbol_img.get_rect(center=(rect.centerx, rect.y + 15)))

    draw_text(screen, fonts["tiny"], "Tap an element", GRID_X, HEIGHT - 17, (82, 92, 108))


def element_at_point(model, pos):
    for element in model.all_elements():
        if cell_rect(element).collidepoint(pos):
            return element
    return None


def main():
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("BitSlate Periodic Table")
    clock = pygame.time.Clock()
    model = PeriodicTableModel()
    fonts = {
        "title": pygame.font.SysFont("arial", 24),
        "symbol": pygame.font.SysFont("arial", 46, bold=True),
        "name": pygame.font.SysFont("arial", 18),
        "small": pygame.font.SysFont("arial", 14),
        "tiny": pygame.font.SysFont("arial", 12),
        "cell": pygame.font.SysFont("arial", 13, bold=True),
        "micro": pygame.font.SysFont("arial", 8),
    }

    running = True
    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False
            elif event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE:
                running = False
            elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                element = element_at_point(model, event.pos)
                if element is not None:
                    model.select_element(element["atomic_number"])

        screen.fill((232, 238, 246))
        draw_info_panel(screen, model, fonts)
        draw_table(screen, model, fonts)
        pygame.display.flip()
        clock.tick(30)

    pygame.quit()


if __name__ == "__main__":
    main()


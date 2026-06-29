import pygame

from atom_model import AtomModel


WIDTH = 480
HEIGHT = 320


def draw_text(surface, font, text, x, y, color=(30, 36, 48)):
    surface.blit(font.render(text, True, color), (x, y))


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
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_p:
                    atom.add_proton()
                elif event.key == pygame.K_n:
                    atom.add_neutron()
                elif event.key == pygame.K_e:
                    atom.add_electron()
                elif event.key == pygame.K_r:
                    atom.reset()

        screen.fill((223, 235, 255))
        pygame.draw.rect(screen, (245, 247, 250), (0, 0, 96, HEIGHT))
        pygame.draw.line(screen, (160, 170, 185), (96, 0), (96, HEIGHT), 2)

        draw_text(screen, font, "Build an Atom", 118, 18)
        draw_text(screen, small_font, "Prototype scaffold", 118, 44)
        draw_text(screen, small_font, "Keys: P proton, N neutron", 112, 270)
        draw_text(screen, small_font, "E electron, R reset", 112, 292)

        draw_text(screen, font, "Proton", 18, 42)
        draw_text(screen, font, "Neutron", 16, 104)
        draw_text(screen, font, "Electron", 14, 166)

        center = (275, 150)
        pygame.draw.circle(screen, (205, 220, 250), center, 78, 2)
        pygame.draw.circle(screen, (185, 205, 245), center, 48, 2)
        pygame.draw.circle(screen, (236, 196, 113), center, 34)
        pygame.draw.circle(screen, (118, 86, 56), center, 34, 2)
        draw_text(screen, font, atom.element_symbol(), center[0] - 8, center[1] - 13, (35, 35, 35))

        info_x = 366
        draw_text(screen, small_font, atom.element_name(), info_x, 84)
        draw_text(screen, small_font, f"Atomic #: {atom.atomic_number()}", info_x, 116)
        draw_text(screen, small_font, f"Mass #: {atom.mass_number()}", info_x, 142)
        draw_text(screen, small_font, f"Charge: {atom.charge()}", info_x, 168)

        draw_text(screen, small_font, f"p: {atom.protons}", 18, 72)
        draw_text(screen, small_font, f"n: {atom.neutrons}", 18, 134)
        draw_text(screen, small_font, f"e: {atom.electrons}", 18, 196)

        pygame.display.flip()
        clock.tick(30)

    pygame.quit()


if __name__ == "__main__":
    main()


from __future__ import annotations

import sys

import pygame

from circuit_model import CircuitModel, ComponentType


WIDTH = 480
HEIGHT = 320
SIDEBAR_WIDTH = 90
FPS = 60

COLOR_BG = (165, 194, 244)
COLOR_SIDEBAR = (238, 238, 238)
COLOR_PANEL_LINE = (170, 170, 170)
COLOR_TEXT = (18, 18, 18)
COLOR_WIRE = (105, 62, 42)
COLOR_TERMINAL = (238, 248, 255)
COLOR_TERMINAL_OUTLINE = (88, 140, 170)
COLOR_SELECTED = (206, 226, 255)
COLOR_STATUS = (40, 64, 92)


TOOLS = [
    ("battery", "Battery"),
    ("bulb", "Bulb"),
    ("wire", "Wire"),
    ("clear", "Clear"),
]


class CircuitLabApp:
    def __init__(self) -> None:
        pygame.init()
        pygame.display.set_caption("BitSlate Circuit Lab Prototype")
        self.screen = pygame.display.set_mode((WIDTH, HEIGHT))
        self.clock = pygame.time.Clock()
        self.font = pygame.font.SysFont("arial", 18)
        self.small_font = pygame.font.SysFont("arial", 14)
        self.model = CircuitModel()
        self.selected_tool = "battery"
        self.status = "Select Battery/Bulb/Wire"
        self.wire_start_terminal_id: int | None = None
        self.wire_preview_pos: tuple[int, int] | None = None
        self.dragging_component_id: int | None = None
        self.drag_offset = (0, 0)

    def run(self) -> None:
        while True:
            self.handle_events()
            self.draw()
            pygame.display.flip()
            self.clock.tick(FPS)

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
                self.on_mouse_up(event.pos)

    def on_mouse_down(self, pos: tuple[int, int]) -> None:
        x, y = pos

        if x < SIDEBAR_WIDTH:
            tool = self.tool_at(y)
            if tool == "clear":
                self.model.clear()
                self.status = "Scene cleared"
            elif tool:
                self.selected_tool = tool
                self.status = f"{tool.title()} tool selected"
            return

        if self.selected_tool in ("battery", "bulb"):
            self.model.add_component(self.selected_tool, x, y)
            self.status = f"Placed {self.selected_tool}"
            return

        if self.selected_tool == "wire":
            terminal = self.model.find_nearest_terminal(x, y)
            if terminal:
                self.wire_start_terminal_id = terminal.id
                self.wire_preview_pos = pos
                self.status = "Drag wire to another terminal"
            else:
                component = self.model.component_at(x, y)
                if component:
                    self.start_drag(component.id, pos)
                else:
                    self.status = "Start wire near a terminal"
            return

        component = self.model.component_at(x, y)
        if component:
            self.start_drag(component.id, pos)

    def on_mouse_motion(self, pos: tuple[int, int]) -> None:
        if self.wire_start_terminal_id is not None:
            self.wire_preview_pos = pos
        elif self.dragging_component_id is not None:
            x, y = pos
            offset_x, offset_y = self.drag_offset
            self.model.move_component(self.dragging_component_id, x - offset_x, y - offset_y)

    def on_mouse_up(self, pos: tuple[int, int]) -> None:
        if self.wire_start_terminal_id is not None:
            terminal = self.model.find_nearest_terminal(pos[0], pos[1])
            if terminal and self.model.add_wire(self.wire_start_terminal_id, terminal.id):
                self.status = "Wire connected"
            else:
                self.status = "Wire not connected"
            self.wire_start_terminal_id = None
            self.wire_preview_pos = None

        self.dragging_component_id = None

    def start_drag(self, component_id: int, pos: tuple[int, int]) -> None:
        component = self.model.get_component(component_id)
        if component is None:
            return
        self.dragging_component_id = component_id
        self.drag_offset = (pos[0] - component.x, pos[1] - component.y)
        self.status = "Dragging component"

    def tool_at(self, y: int) -> str | None:
        for index, (tool, _label) in enumerate(TOOLS):
            top = 12 + index * 65
            if top <= y <= top + 55:
                return tool
        return None

    def draw(self) -> None:
        self.screen.fill(COLOR_BG)
        self.draw_sidebar()
        self.draw_wires()
        self.draw_components()
        self.draw_wire_preview()
        self.draw_status()

    def draw_sidebar(self) -> None:
        pygame.draw.rect(self.screen, COLOR_SIDEBAR, (0, 0, SIDEBAR_WIDTH, HEIGHT))
        pygame.draw.line(self.screen, COLOR_PANEL_LINE, (SIDEBAR_WIDTH, 0), (SIDEBAR_WIDTH, HEIGHT), 2)

        for index, (tool, label) in enumerate(TOOLS):
            rect = pygame.Rect(8, 12 + index * 65, SIDEBAR_WIDTH - 16, 55)
            fill = COLOR_SELECTED if self.selected_tool == tool else (248, 248, 248)
            pygame.draw.rect(self.screen, fill, rect, border_radius=6)
            pygame.draw.rect(self.screen, COLOR_PANEL_LINE, rect, 1, border_radius=6)
            self.draw_tool_icon(tool, rect.centerx, rect.y + 20)
            text = self.small_font.render(label, True, COLOR_TEXT)
            self.screen.blit(text, text.get_rect(center=(rect.centerx, rect.y + 42)))

    def draw_tool_icon(self, tool: str, x: int, y: int) -> None:
        if tool == "battery":
            pygame.draw.rect(self.screen, (35, 35, 35), (x - 22, y - 7, 28, 14))
            pygame.draw.rect(self.screen, (242, 174, 60), (x + 6, y - 7, 18, 14))
            pygame.draw.line(self.screen, (255, 255, 255), (x + 20, y - 10), (x + 20, y + 10), 2)
        elif tool == "bulb":
            pygame.draw.circle(self.screen, (255, 245, 170), (x, y - 3), 13, 2)
            pygame.draw.rect(self.screen, (120, 120, 120), (x - 8, y + 9, 16, 8))
            pygame.draw.line(self.screen, (202, 80, 55), (x - 5, y - 2), (x, y + 5), 2)
            pygame.draw.line(self.screen, (202, 80, 55), (x + 5, y - 2), (x, y + 5), 2)
        elif tool == "wire":
            pygame.draw.line(self.screen, COLOR_WIRE, (x - 24, y), (x + 24, y), 6)
            pygame.draw.circle(self.screen, COLOR_WIRE, (x - 24, y), 5)
            pygame.draw.circle(self.screen, COLOR_WIRE, (x + 24, y), 5)
        elif tool == "clear":
            pygame.draw.line(self.screen, (160, 50, 50), (x - 12, y - 12), (x + 12, y + 12), 4)
            pygame.draw.line(self.screen, (160, 50, 50), (x + 12, y - 12), (x - 12, y + 12), 4)

    def draw_wires(self) -> None:
        for wire in self.model.wires:
            start = self.model.get_terminal_world_pos(wire.from_terminal_id)
            end = self.model.get_terminal_world_pos(wire.to_terminal_id)
            if start and end:
                pygame.draw.line(self.screen, COLOR_WIRE, start, end, 7)
                pygame.draw.circle(self.screen, COLOR_WIRE, start, 5)
                pygame.draw.circle(self.screen, COLOR_WIRE, end, 5)

    def draw_components(self) -> None:
        lit_bulbs = self.model.lit_bulb_ids()
        for component in self.model.components:
            if component.type == ComponentType.BATTERY:
                self.draw_battery(component)
            elif component.type == ComponentType.BULB:
                self.draw_bulb(component, component.id in lit_bulbs)

            for terminal in component.terminals:
                pos = self.model.get_terminal_world_pos(terminal.id)
                if pos:
                    pygame.draw.circle(self.screen, COLOR_TERMINAL, pos, 7)
                    pygame.draw.circle(self.screen, COLOR_TERMINAL_OUTLINE, pos, 7, 2)

    def draw_battery(self, component) -> None:
        x, y = component.x, component.y
        pygame.draw.rect(self.screen, (35, 35, 35), (x - 38, y - 16, 38, 32), border_radius=3)
        pygame.draw.rect(self.screen, (240, 166, 50), (x, y - 16, 38, 32), border_radius=3)
        pygame.draw.line(self.screen, (255, 255, 255), (x + 29, y - 21), (x + 29, y + 21), 3)
        self.draw_centered_text("-", x - 24, y, self.font, (255, 255, 255))
        self.draw_centered_text("+", x + 20, y, self.font, (255, 255, 255))

    def draw_bulb(self, component, lit: bool) -> None:
        x, y = component.x, component.y
        if lit:
            pygame.draw.circle(self.screen, (255, 234, 84), (x, y), 36)
            pygame.draw.circle(self.screen, (255, 249, 160), (x, y), 27)
        else:
            pygame.draw.circle(self.screen, (224, 224, 206), (x, y), 28)
        pygame.draw.circle(self.screen, (105, 105, 95), (x, y), 28, 3)
        pygame.draw.rect(self.screen, (135, 135, 125), (x - 13, y + 22, 26, 14), border_radius=2)
        pygame.draw.line(self.screen, (207, 94, 48), (x - 8, y - 2), (x, y + 12), 3)
        pygame.draw.line(self.screen, (207, 94, 48), (x + 8, y - 2), (x, y + 12), 3)

    def draw_wire_preview(self) -> None:
        if self.wire_start_terminal_id is None or self.wire_preview_pos is None:
            return

        start = self.model.get_terminal_world_pos(self.wire_start_terminal_id)
        if start:
            pygame.draw.line(self.screen, (135, 86, 55), start, self.wire_preview_pos, 5)
            pygame.draw.circle(self.screen, (135, 86, 55), start, 5)

    def draw_status(self) -> None:
        if self.model.is_bulb_lit():
            status = "Circuit complete: bulb ON"
            color = (0, 105, 45)
        elif self.model.components or self.model.wires:
            status = "Circuit incomplete"
            color = COLOR_STATUS
        else:
            status = self.status
            color = COLOR_STATUS

        pygame.draw.rect(self.screen, (230, 238, 250), (SIDEBAR_WIDTH + 8, HEIGHT - 30, 220, 22), border_radius=5)
        text = self.small_font.render(status, True, color)
        self.screen.blit(text, (SIDEBAR_WIDTH + 16, HEIGHT - 25))

    def draw_centered_text(self, text: str, x: int, y: int, font: pygame.font.Font, color: tuple[int, int, int]) -> None:
        surface = font.render(text, True, color)
        self.screen.blit(surface, surface.get_rect(center=(x, y)))

    def quit(self) -> None:
        pygame.quit()
        sys.exit(0)


if __name__ == "__main__":
    CircuitLabApp().run()

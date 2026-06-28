from __future__ import annotations

from collections import deque
from dataclasses import dataclass, field
from enum import Enum
from itertools import count
from math import hypot


SNAP_DISTANCE = 15


class ComponentType(Enum):
    BATTERY = "battery"
    BULB = "bulb"


@dataclass
class Terminal:
    id: int
    component_id: int
    name: str
    local_x: int
    local_y: int


@dataclass
class Component:
    id: int
    type: ComponentType
    x: int
    y: int
    terminals: list[Terminal] = field(default_factory=list)


@dataclass
class Wire:
    from_terminal_id: int
    to_terminal_id: int


class CircuitModel:
    def __init__(self) -> None:
        self.components: list[Component] = []
        self.wires: list[Wire] = []
        self._component_ids = count(1)
        self._terminal_ids = count(1)

    def clear(self) -> None:
        self.components.clear()
        self.wires.clear()
        self._component_ids = count(1)
        self._terminal_ids = count(1)

    def add_component(self, component_type: ComponentType | str, x: int, y: int) -> Component:
        normalized_type = self._normalize_component_type(component_type)
        component_id = next(self._component_ids)
        component = Component(component_id, normalized_type, x, y)

        if normalized_type == ComponentType.BATTERY:
            component.terminals = [
                self._new_terminal(component_id, "negative", -32, 0),
                self._new_terminal(component_id, "positive", 32, 0),
            ]
        elif normalized_type == ComponentType.BULB:
            component.terminals = [
                self._new_terminal(component_id, "a", -24, 28),
                self._new_terminal(component_id, "b", 24, 28),
            ]

        self.components.append(component)
        return component

    def add_wire(self, from_terminal_id: int, to_terminal_id: int) -> bool:
        if from_terminal_id == to_terminal_id:
            return False
        if self._find_terminal(from_terminal_id) is None or self._find_terminal(to_terminal_id) is None:
            return False
        if self._wire_exists(from_terminal_id, to_terminal_id):
            return False

        self.wires.append(Wire(from_terminal_id, to_terminal_id))
        return True

    def move_component(self, component_id: int, x: int, y: int) -> None:
        component = self.get_component(component_id)
        if component:
            component.x = x
            component.y = y

    def get_component(self, component_id: int) -> Component | None:
        return next((component for component in self.components if component.id == component_id), None)

    def get_terminal_world_pos(self, terminal_id: int) -> tuple[int, int] | None:
        terminal = self._find_terminal(terminal_id)
        if terminal is None:
            return None

        component = self.get_component(terminal.component_id)
        if component is None:
            return None

        return component.x + terminal.local_x, component.y + terminal.local_y

    def find_nearest_terminal(self, x: int, y: int, max_distance: int = SNAP_DISTANCE) -> Terminal | None:
        best_terminal = None
        best_distance = max_distance

        for component in self.components:
            for terminal in component.terminals:
                terminal_pos = self.get_terminal_world_pos(terminal.id)
                if terminal_pos is None:
                    continue
                terminal_x, terminal_y = terminal_pos
                distance = hypot(terminal_x - x, terminal_y - y)
                if distance <= best_distance:
                    best_distance = distance
                    best_terminal = terminal

        return best_terminal

    def component_at(self, x: int, y: int) -> Component | None:
        for component in reversed(self.components):
            if component.type == ComponentType.BATTERY:
                if component.x - 40 <= x <= component.x + 40 and component.y - 22 <= y <= component.y + 22:
                    return component
            elif component.type == ComponentType.BULB:
                if hypot(component.x - x, component.y - y) <= 34:
                    return component
        return None

    def lit_bulb_ids(self) -> set[int]:
        lit: set[int] = set()
        batteries = [component for component in self.components if component.type == ComponentType.BATTERY]
        bulbs = [component for component in self.components if component.type == ComponentType.BULB]

        for bulb in bulbs:
            for battery in batteries:
                if self._battery_can_reach_through_bulb(battery, bulb.id):
                    lit.add(bulb.id)
                    break

        return lit

    def is_bulb_lit(self, bulb_id: int | None = None) -> bool:
        lit = self.lit_bulb_ids()
        if bulb_id is None:
            return bool(lit)
        return bulb_id in lit

    def get_status_text(self) -> str:
        if self.is_bulb_lit():
            return "Circuit complete: bulb ON"
        if self.components or self.wires:
            return "Circuit incomplete"
        return "Select Battery/Bulb/Wire"

    def _battery_can_reach_through_bulb(self, battery: Component, bulb_id: int) -> bool:
        positive = self._terminal_by_name(battery, "positive")
        negative = self._terminal_by_name(battery, "negative")
        if positive is None or negative is None:
            return False

        graph = self._build_graph()
        queue = deque([(positive.id, False)])
        visited = {(positive.id, False)}

        while queue:
            terminal_id, used_target_bulb = queue.popleft()
            if terminal_id == negative.id and used_target_bulb:
                return True

            for next_terminal_id, edge_bulb_id in graph.get(terminal_id, []):
                next_used = used_target_bulb or edge_bulb_id == bulb_id
                state = (next_terminal_id, next_used)
                if state not in visited:
                    visited.add(state)
                    queue.append(state)

        return False

    def _build_graph(self) -> dict[int, list[tuple[int, int | None]]]:
        graph: dict[int, list[tuple[int, int | None]]] = {}

        def add_edge(a: int, b: int, bulb_id: int | None = None) -> None:
            graph.setdefault(a, []).append((b, bulb_id))
            graph.setdefault(b, []).append((a, bulb_id))

        for wire in self.wires:
            add_edge(wire.from_terminal_id, wire.to_terminal_id)

        for component in self.components:
            if component.type == ComponentType.BULB and len(component.terminals) == 2:
                add_edge(component.terminals[0].id, component.terminals[1].id, component.id)

        return graph

    def _new_terminal(self, component_id: int, name: str, local_x: int, local_y: int) -> Terminal:
        return Terminal(next(self._terminal_ids), component_id, name, local_x, local_y)

    def _find_terminal(self, terminal_id: int) -> Terminal | None:
        for component in self.components:
            for terminal in component.terminals:
                if terminal.id == terminal_id:
                    return terminal
        return None

    def _terminal_by_name(self, component: Component, name: str) -> Terminal | None:
        return next((terminal for terminal in component.terminals if terminal.name == name), None)

    def _wire_exists(self, a: int, b: int) -> bool:
        return any(
            {wire.from_terminal_id, wire.to_terminal_id} == {a, b}
            for wire in self.wires
        )

    def _normalize_component_type(self, component_type: ComponentType | str) -> ComponentType:
        if isinstance(component_type, ComponentType):
            return component_type
        try:
            return ComponentType(component_type)
        except ValueError as error:
            raise ValueError(f"Unsupported component type: {component_type}") from error

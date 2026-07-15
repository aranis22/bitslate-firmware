from dataclasses import dataclass, field
from uuid import uuid4

@dataclass
class BlockDefinition:
    opcode: str
    category: str
    shape: str
    parts: list
    defaults: dict = field(default_factory=dict)

@dataclass
class BlockModel:
    opcode: str
    category: str
    arguments: dict
    x: float = 0
    y: float = 0
    next_block: str | None = None
    parent_block: str | None = None
    id: str = field(default_factory=lambda: str(uuid4()))

    def to_dict(self):
        return {"id": self.id, "opcode": self.opcode, "category": self.category,
                "arguments": dict(self.arguments), "x": self.x, "y": self.y,
                "next_block": self.next_block, "parent_block": self.parent_block}

BLOCKS = {
 "movement": [
  BlockDefinition("motion_move", "movement", "stack", ["move ", ("steps", "number"), " steps"], {"steps": "10"}),
  BlockDefinition("motion_turn", "movement", "stack", ["turn ", ("degrees", "number"), " degrees"], {"degrees": "15"}),
  BlockDefinition("motion_goto", "movement", "stack", ["go to x: ", ("x", "number"), " y: ", ("y", "number")], {"x": "0", "y": "0"}),
  BlockDefinition("motion_direction", "movement", "stack", ["set direction ", ("direction", "number")], {"direction": "90"})],
 "events": [
  BlockDefinition("event_start", "events", "hat", ["when ", ("run_button", "play_icon"), " clicked"]),
  BlockDefinition("event_button", "events", "hat", ["when ", ("button", "text"), " pressed"], {"button": "button"}),
  BlockDefinition("event_touch", "events", "hat", ["when screen touched"]),
  BlockDefinition("event_receive", "events", "hat", ["when I receive ", ("message", "text")], {"message": "message"})],
 "control": [
  BlockDefinition("control_wait", "control", "stack", ["wait ", ("seconds", "number"), " seconds"], {"seconds": "0.5"}),
  BlockDefinition("control_repeat", "control", "c", ["repeat ", ("times", "number"), " times"], {"times": "10"}),
  BlockDefinition("control_forever", "control", "c", ["forever"]),
  BlockDefinition("control_if", "control", "c", ["if ", ("condition", "text"), " then"], {"condition": "condition"})],
 "operators": [
  BlockDefinition("operator_add", "operators", "reporter", [("left", "number"), " + ", ("right", "number")], {"left": "0", "right": "0"}),
  BlockDefinition("operator_subtract", "operators", "reporter", [("left", "number"), " − ", ("right", "number")], {"left": "0", "right": "0"}),
  BlockDefinition("operator_gt", "operators", "reporter", [("left", "number"), " > ", ("right", "number")], {"left": "0", "right": "0"}),
  BlockDefinition("operator_random", "operators", "reporter", ["pick random ", ("from", "number"), " to ", ("to", "number")], {"from": "1", "to": "10"})],
 "camera": []
}

DEFINITIONS = {b.opcode: b for group in BLOCKS.values() for b in group}

def make_model(definition):
    return BlockModel(definition.opcode, definition.category, dict(definition.defaults))

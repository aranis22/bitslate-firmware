#include "CircuitModel.h"

#include <cstring>

void CircuitModel::clear() {
  componentCount = 0;
  terminalCount = 0;
  wireCount = 0;
  nextComponentId = 1;
  nextTerminalId = 1;
}

int CircuitModel::addComponent(ComponentType type, int x, int y) {
  if (componentCount >= MaxComponents || terminalCount + 2 > MaxTerminals) {
    return 0;
  }

  Component& component = components[componentCount++];
  component.id = nextComponentId++;
  component.type = type;
  component.x = x;
  component.y = y;

  if (type == ComponentType::Battery) {
    component.terminalIds[0] = addTerminal(component.id, "negative", -32, 0);
    component.terminalIds[1] = addTerminal(component.id, "positive", 32, 0);
  } else {
    component.terminalIds[0] = addTerminal(component.id, "a", -24, 28);
    component.terminalIds[1] = addTerminal(component.id, "b", 24, 28);
  }

  return component.id;
}

bool CircuitModel::addWire(int fromTerminalId, int toTerminalId) {
  if (fromTerminalId == toTerminalId || wireCount >= MaxWires) {
    return false;
  }
  if (findTerminal(fromTerminalId) == nullptr || findTerminal(toTerminalId) == nullptr) {
    return false;
  }
  if (wireExists(fromTerminalId, toTerminalId)) {
    return false;
  }

  Wire& wire = wires[wireCount++];
  wire.fromTerminalId = fromTerminalId;
  wire.toTerminalId = toTerminalId;
  return true;
}

bool CircuitModel::isBulbLit() const {
  for (int i = 0; i < componentCount; ++i) {
    if (components[i].type == ComponentType::Bulb && isBulbLit(components[i].id)) {
      return true;
    }
  }
  return false;
}

bool CircuitModel::isBulbLit(int bulbComponentId) const {
  const Component* bulb = findComponent(bulbComponentId);
  if (bulb == nullptr || bulb->type != ComponentType::Bulb) {
    return false;
  }

  for (int i = 0; i < componentCount; ++i) {
    if (components[i].type == ComponentType::Battery && batteryCanReachThroughBulb(components[i], bulbComponentId)) {
      return true;
    }
  }
  return false;
}

int CircuitModel::getComponentCount() const {
  return componentCount;
}

int CircuitModel::getWireCount() const {
  return wireCount;
}

const Component* CircuitModel::getComponentAt(int index) const {
  if (index < 0 || index >= componentCount) {
    return nullptr;
  }
  return &components[index];
}

const Wire* CircuitModel::getWireAt(int index) const {
  if (index < 0 || index >= wireCount) {
    return nullptr;
  }
  return &wires[index];
}

int CircuitModel::getTerminalId(int componentId, const char* name) const {
  const Component* component = findComponent(componentId);
  if (component == nullptr) {
    return 0;
  }

  const Terminal* terminal = terminalByName(*component, name);
  return terminal ? terminal->id : 0;
}

bool CircuitModel::getTerminalWorldPos(int terminalId, int* x, int* y) const {
  const Terminal* terminal = findTerminal(terminalId);
  if (terminal == nullptr) {
    return false;
  }

  const Component* component = findComponent(terminal->componentId);
  if (component == nullptr) {
    return false;
  }

  if (x != nullptr) {
    *x = component->x + terminal->localX;
  }
  if (y != nullptr) {
    *y = component->y + terminal->localY;
  }
  return true;
}

int CircuitModel::findNearestTerminal(int x, int y, int maxDistance) const {
  int bestTerminalId = 0;
  int bestDistanceSquared = maxDistance * maxDistance;

  for (int i = 0; i < terminalCount; ++i) {
    int distanceSquared = distanceSquaredToTerminal(terminals[i].id, x, y);
    if (distanceSquared <= bestDistanceSquared) {
      bestDistanceSquared = distanceSquared;
      bestTerminalId = terminals[i].id;
    }
  }

  return bestTerminalId;
}

int CircuitModel::addTerminal(int componentId, const char* name, int localX, int localY) {
  if (terminalCount >= MaxTerminals) {
    return 0;
  }

  Terminal& terminal = terminals[terminalCount++];
  terminal.id = nextTerminalId++;
  terminal.componentId = componentId;
  terminal.name = name;
  terminal.localX = localX;
  terminal.localY = localY;
  return terminal.id;
}

const Component* CircuitModel::findComponent(int componentId) const {
  for (int i = 0; i < componentCount; ++i) {
    if (components[i].id == componentId) {
      return &components[i];
    }
  }
  return nullptr;
}

const Terminal* CircuitModel::findTerminal(int terminalId) const {
  for (int i = 0; i < terminalCount; ++i) {
    if (terminals[i].id == terminalId) {
      return &terminals[i];
    }
  }
  return nullptr;
}

const Terminal* CircuitModel::terminalByName(const Component& component, const char* name) const {
  for (int i = 0; i < 2; ++i) {
    const Terminal* terminal = findTerminal(component.terminalIds[i]);
    if (terminal != nullptr && terminalNameEquals(*terminal, name)) {
      return terminal;
    }
  }
  return nullptr;
}

bool CircuitModel::wireExists(int a, int b) const {
  for (int i = 0; i < wireCount; ++i) {
    if ((wires[i].fromTerminalId == a && wires[i].toTerminalId == b) ||
        (wires[i].fromTerminalId == b && wires[i].toTerminalId == a)) {
      return true;
    }
  }
  return false;
}

bool CircuitModel::batteryCanReachThroughBulb(const Component& battery, int bulbComponentId) const {
  const Terminal* positive = terminalByName(battery, "positive");
  const Terminal* negative = terminalByName(battery, "negative");
  if (positive == nullptr || negative == nullptr) {
    return false;
  }

  GraphEdge graph[MaxTerminals][MaxTerminals] = {};
  int edgeCounts[MaxTerminals] = {};

  for (int i = 0; i < wireCount; ++i) {
    addGraphEdge(graph, edgeCounts, wires[i].fromTerminalId, wires[i].toTerminalId, 0);
  }

  for (int i = 0; i < componentCount; ++i) {
    const Component& component = components[i];
    if (component.type == ComponentType::Bulb) {
      addGraphEdge(graph, edgeCounts, component.terminalIds[0], component.terminalIds[1], component.id);
    }
  }

  struct SearchState {
    int terminalId;
    bool usedTargetBulb;
  };

  SearchState queue[MaxTerminals * 2] = {};
  bool visited[MaxTerminals][2] = {};
  int head = 0;
  int tail = 0;

  int positiveIndex = terminalIndex(positive->id);
  if (positiveIndex < 0) {
    return false;
  }

  queue[tail++] = {positive->id, false};
  visited[positiveIndex][0] = true;

  while (head < tail) {
    SearchState state = queue[head++];
    if (state.terminalId == negative->id && state.usedTargetBulb) {
      return true;
    }

    int fromIndex = terminalIndex(state.terminalId);
    if (fromIndex < 0) {
      continue;
    }

    for (int i = 0; i < edgeCounts[fromIndex]; ++i) {
      const GraphEdge& edge = graph[fromIndex][i];
      bool nextUsed = state.usedTargetBulb || edge.bulbComponentId == bulbComponentId;
      int nextIndex = terminalIndex(edge.toTerminalId);
      if (nextIndex >= 0 && !visited[nextIndex][nextUsed ? 1 : 0] && tail < MaxTerminals * 2) {
        visited[nextIndex][nextUsed ? 1 : 0] = true;
        queue[tail++] = {edge.toTerminalId, nextUsed};
      }
    }
  }

  return false;
}

bool CircuitModel::terminalNameEquals(const Terminal& terminal, const char* name) const {
  return std::strcmp(terminal.name, name) == 0;
}

void CircuitModel::addGraphEdge(
    GraphEdge graph[MaxTerminals][MaxTerminals],
    int edgeCounts[MaxTerminals],
    int from,
    int to,
    int bulbComponentId) const {
  int fromIndex = terminalIndex(from);
  int toIndex = terminalIndex(to);
  if (fromIndex < 0 || toIndex < 0) {
    return;
  }
  if (edgeCounts[fromIndex] < MaxTerminals) {
    GraphEdge& edge = graph[fromIndex][edgeCounts[fromIndex]++];
    edge.toTerminalId = to;
    edge.bulbComponentId = bulbComponentId;
  }
  if (edgeCounts[toIndex] < MaxTerminals) {
    GraphEdge& edge = graph[toIndex][edgeCounts[toIndex]++];
    edge.toTerminalId = from;
    edge.bulbComponentId = bulbComponentId;
  }
}

int CircuitModel::terminalIndex(int terminalId) const {
  for (int i = 0; i < terminalCount; ++i) {
    if (terminals[i].id == terminalId) {
      return i;
    }
  }
  return -1;
}

int CircuitModel::distanceSquaredToTerminal(int terminalId, int x, int y) const {
  int terminalX = 0;
  int terminalY = 0;
  if (!getTerminalWorldPos(terminalId, &terminalX, &terminalY)) {
    return 0x7FFFFFFF;
  }

  int dx = terminalX - x;
  int dy = terminalY - y;
  return dx * dx + dy * dy;
}

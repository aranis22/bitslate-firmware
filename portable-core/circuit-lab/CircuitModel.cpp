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

    wires[wireCount++] = {fromTerminalId, toTerminalId};
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

int CircuitModel::getTerminalId(int componentId, const char* name) const {
    const Component* component = findComponent(componentId);
    if (component == nullptr) {
        return 0;
    }

    const Terminal* terminal = terminalByName(*component, name);
    return terminal ? terminal->id : 0;
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

    const int positiveIndex = terminalIndex(positive->id);
    if (positiveIndex < 0) {
        return false;
    }

    queue[tail++] = {positive->id, false};
    visited[positiveIndex][0] = true;

    while (head < tail) {
        const SearchState state = queue[head++];
        if (state.terminalId == negative->id && state.usedTargetBulb) {
            return true;
        }

        const int fromIndex = terminalIndex(state.terminalId);
        if (fromIndex < 0) {
            continue;
        }

        for (int i = 0; i < edgeCounts[fromIndex]; ++i) {
            const GraphEdge& edge = graph[fromIndex][i];
            const bool nextUsed = state.usedTargetBulb || edge.bulbComponentId == bulbComponentId;
            const int nextIndex = terminalIndex(edge.toTerminalId);
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
    int bulbComponentId
) const {
    const int fromIndex = terminalIndex(from);
    const int toIndex = terminalIndex(to);
    if (fromIndex < 0 || toIndex < 0) {
        return;
    }
    if (edgeCounts[fromIndex] < MaxTerminals) {
        graph[fromIndex][edgeCounts[fromIndex]++] = {to, bulbComponentId};
    }
    if (edgeCounts[toIndex] < MaxTerminals) {
        graph[toIndex][edgeCounts[toIndex]++] = {from, bulbComponentId};
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

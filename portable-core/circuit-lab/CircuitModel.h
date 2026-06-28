#pragma once

enum class ComponentType {
    Battery,
    Bulb
};

struct Terminal {
    int id = 0;
    int componentId = 0;
    const char* name = "";
    int localX = 0;
    int localY = 0;
};

struct Component {
    int id = 0;
    ComponentType type = ComponentType::Battery;
    int x = 0;
    int y = 0;
    int terminalIds[2] = {0, 0};
};

struct Wire {
    int fromTerminalId = 0;
    int toTerminalId = 0;
};

class CircuitModel {
public:
    static constexpr int MaxComponents = 8;
    static constexpr int MaxTerminals = 16;
    static constexpr int MaxWires = 16;

    void clear();
    int addComponent(ComponentType type, int x, int y);
    bool addWire(int fromTerminalId, int toTerminalId);
    bool isBulbLit() const;
    bool isBulbLit(int bulbComponentId) const;
    int getComponentCount() const;
    int getWireCount() const;
    int getTerminalId(int componentId, const char* name) const;

private:
    struct GraphEdge {
        int toTerminalId = 0;
        int bulbComponentId = 0;
    };

    Component components[MaxComponents] = {};
    Terminal terminals[MaxTerminals] = {};
    Wire wires[MaxWires] = {};
    int componentCount = 0;
    int terminalCount = 0;
    int wireCount = 0;
    int nextComponentId = 1;
    int nextTerminalId = 1;

    int addTerminal(int componentId, const char* name, int localX, int localY);
    const Component* findComponent(int componentId) const;
    const Terminal* findTerminal(int terminalId) const;
    const Terminal* terminalByName(const Component& component, const char* name) const;
    bool wireExists(int a, int b) const;
    bool batteryCanReachThroughBulb(const Component& battery, int bulbComponentId) const;
    bool terminalNameEquals(const Terminal& terminal, const char* name) const;
    void addGraphEdge(GraphEdge graph[MaxTerminals][MaxTerminals], int edgeCounts[MaxTerminals], int from, int to, int bulbComponentId) const;
    int terminalIndex(int terminalId) const;
};

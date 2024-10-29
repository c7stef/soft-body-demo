#ifndef GRAPH_FORCE_SYSTEM_HPP
#define GRAPH_FORCE_SYSTEM_HPP

#include "graf.hpp"

#include <SFML/Graphics.hpp>
#include <functional>

class Graf;

class GraphForceSystem
{
public:
    GraphForceSystem(Graf& graph) :
        graf { graph }
    {
    }

    void update();
    void reload();

    void sendLeftButtonPressed(sf::Vector2f coords);
    void sendLeftButtonReleased(sf::Vector2f coords);
    void sendMouseMoved(sf::Vector2f coords);

private:
    Graf& graf;

    bool dragging { false };

    int draggedNodeIndex {};
    sf::Vector2f draggedNodeInitialPos {};
    sf::Vector2f draggedNodeInitialMousePos {};
    sf::Vector2f draggedNodeTargetPos {};

    bool isDragged(int index) const { return dragging && index == draggedNodeIndex; }

    static constexpr float springConstant = 4.f;
    static constexpr float dampingConstant = 2.0f;
    static constexpr float restDistance = 170.0f;
    static constexpr float airResistance = 2.5f;
    static constexpr float fieldScale = 8e5f;

    static constexpr float stepSize = 0.1f;

    // static constexpr float dragSmoothness = 0.4f;

    class SystemState
    {
    public:
        SystemState(Graf::NoduriSSize count, const GraphForceSystem& forceSystem);

        float& x(int index);
        float& y(int index);
        float& xDot(int index);
        float& yDot(int index);

        void next(const Graf& graf);

    private:
        SystemState getDiffs(const Graf& graf);

        Graf::NoduriSSize count;
        std::vector<float> values;

        std::reference_wrapper<const GraphForceSystem> forceSystem;
    };

    SystemState state{graf.nodeCount(), *this};

    friend SystemState;
};

#endif // GRAPH_FORCE_SYSTEM_HPP

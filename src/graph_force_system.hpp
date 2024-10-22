#ifndef GRAPH_FORCE_SYSTEM_HPP
#define GRAPH_FORCE_SYSTEM_HPP

#include "graf.hpp"

#include <SFML/Graphics.hpp>

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

    bool isDragged(int index) { return dragging && index == draggedNodeIndex; }

    static constexpr float springConstant = 5.f;
    static constexpr float dampingConstant = .3f;
    static constexpr float restDistance = 170.0f;
    static constexpr float stepSize = 0.1f;

    // static constexpr float dragSmoothness = 0.4f;

    class SystemState
    {
    public:
        SystemState(Graf::NoduriSSize count);

        float& x(int index);
        float& y(int index);
        float& xDot(int index);
        float& yDot(int index);

        void next(const Graf& graf);

    private:
        SystemState getDiffs(const Graf& graf);

        Graf::NoduriSSize count;
        std::vector<float> values;
    };

    SystemState state{graf.nodeCount()};
};

#endif // GRAPH_FORCE_SYSTEM_HPP

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

    static constexpr float kelastic = 0.2f;
    static constexpr float distNoduri = 170.0f;

    static constexpr float dragSmoothness = 0.4f;
};

#endif // GRAPH_FORCE_SYSTEM_HPP

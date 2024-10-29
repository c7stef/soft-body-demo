#include "graph_force_system.hpp"

#include "nod.hpp"
#include "utilities.hpp"
#include <SFML/System/Vector2.hpp>
#include <utility>

GraphForceSystem::SystemState::SystemState(Graf::NoduriSSize nodeCount, const GraphForceSystem& forceSystem)
    : count{nodeCount}, values(count * 4), forceSystem{forceSystem}
{
}

float& GraphForceSystem::SystemState::x(int index)
{
    return values[index * 4];
}

float& GraphForceSystem::SystemState::y(int index)
{
    return values[index * 4 + 1];
}

float& GraphForceSystem::SystemState::xDot(int index)
{
    return values[index * 4 + 2];
}

float& GraphForceSystem::SystemState::yDot(int index)
{
    return values[index * 4 + 3];
}

static float distanceAdjusted(sf::Vector2f a, sf::Vector2f b)
{
    constexpr float offset = 20.f;
    return Util::distance(a, b) + offset;
}

static sf::Vector2f springForce(
    sf::Vector2f targetPos,
    sf::Vector2f targetVelocity,
    sf::Vector2f sourcePos,
    sf::Vector2f sourceVelocity,
    float restDistance,
    float springConstant,
    float dampingConstant
) {
    float dist { distanceAdjusted(
        { targetPos.x, targetPos.y },
        { sourcePos.x, sourcePos.y }
    ) };
    auto distSquared { dist * dist };

    float stretch = restDistance / dist;

    auto computeDeltas = [&](float targetVal, float sourceVal, float targetValDot, float sourceValDot)
    {
        float delta = (targetVal - sourceVal) * (1 - stretch);
        float deltaDot = (targetValDot - sourceValDot) * (1 - stretch)
            + (targetVal - sourceVal) * stretch * (
                (targetPos.x - sourcePos.x) * targetVelocity.x
                + (sourcePos.x - targetPos.x) * sourceVelocity.x
                + (targetPos.y - sourcePos.y) * targetVelocity.y
                + (sourcePos.y - targetPos.y) * sourceVelocity.y
            ) / (distSquared);

        return std::pair { delta, deltaDot };
    };

    auto [xDelta, xDeltaDot] = computeDeltas(targetPos.x, sourcePos.x, targetVelocity.x, sourceVelocity.x);
    auto [yDelta, yDeltaDot] = computeDeltas(targetPos.y, sourcePos.y, targetVelocity.y, sourceVelocity.y);

    return {
        -springConstant * xDelta - dampingConstant * xDeltaDot,
        -springConstant * yDelta - dampingConstant * yDeltaDot
    };
}

static sf::Vector2f electrostaticForce(
    sf::Vector2f targetPos,
    sf::Vector2f sourcePos,
    float fieldScale
) {
    float dist { distanceAdjusted(
        { targetPos.x, targetPos.y },
        { sourcePos.x, sourcePos.y }
    ) };
    
    auto distSquared { dist * dist };

    sf::Vector2f directionAway {
        (targetPos.x - sourcePos.x) / dist,
        (targetPos.y - sourcePos.y) / dist
    };

    return {
        fieldScale * directionAway.x / distSquared,
        fieldScale * directionAway.y / distSquared
    };
}

GraphForceSystem::SystemState GraphForceSystem::SystemState::getDiffs(const Graf& graf)
{
    GraphForceSystem::SystemState diffs { count, forceSystem };
    
    for (int i = 0; i < count; i++) {
        diffs.x(i) = xDot(i);
        diffs.y(i) = yDot(i);

        diffs.xDot(i) = -airResistance * xDot(i);
        diffs.yDot(i) = -airResistance * yDot(i);

        for (int j = 0; j < count; j++) {
            auto electrostatic { electrostaticForce(
                { x(i), y(i) },
                { x(j), y(j) },
                fieldScale
            ) };

            diffs.xDot(i) += electrostatic.x;
            diffs.yDot(i) += electrostatic.y;

            if (i == j || !graf.isEdge(i, j))
                continue;

            auto spring { springForce(
                { x(i), y(i) },
                { xDot(i), yDot(i) },
                { x(j), y(j) },
                { xDot(j), yDot(j) },
                restDistance, springConstant, dampingConstant
            )};

            diffs.xDot(i) += spring.x;
            diffs.yDot(i) += spring.y;
        }

        if (forceSystem.get().isDragged(i)) {
            auto mouseForce { springForce(
                { x(i), y(i) }, 
                { xDot(i), yDot(i) }, 
                {
                    forceSystem.get().draggedNodeTargetPos.x,
                    forceSystem.get().draggedNodeTargetPos.y
                },
                { 0.f, 0.f },
                0.f,
                springConstant * 2,
                dampingConstant
            ) };

            diffs.xDot(i) += mouseForce.x;
            diffs.yDot(i) += mouseForce.y;
        }
    }

    return diffs;
}

void GraphForceSystem::SystemState::next(const Graf& graf)
{
    auto scaleByStepSize = [&](auto& state) {
        for (int i = 0; i < count * 4; i++)
            state.values[i] *= stepSize;
    };

    auto stateK1 { *this };
    auto K1 { stateK1.getDiffs(graf) };
    scaleByStepSize(K1);

    auto stateK2 { stateK1 };
    for (int i = 0; i < count * 4; i++)
        stateK2.values[i] += K1.values[i] / 2;
    auto K2 { stateK2.getDiffs(graf) };
    scaleByStepSize(K2);

    auto stateK3 { stateK1 };
    for (int i = 0; i < count * 4; i++)
        stateK3.values[i] += K2.values[i] / 2;
    auto K3 { stateK3.getDiffs(graf) };
    scaleByStepSize(K3);

    auto stateK4 { stateK1 };
    for (int i = 0; i < count * 4; i++)
        stateK3.values[i] += K3.values[i];
    auto K4 { stateK4.getDiffs(graf) };
    scaleByStepSize(K4);

    for (int i = 0; i < count * 4; i++)
        values[i] += (K1.values[i] + 2. * K2.values[i] + 2. * K3.values[i] + K4.values[i]) / 6.;
}

void GraphForceSystem::reload()
{
    state = SystemState { graf.nodeCount(), *this };
    
    for (int i = 0; i < graf.nodeCount(); i++) {
        state.x(i) = graf.node(i).getPosition().x;
        state.y(i) = graf.node(i).getPosition().y;
        state.xDot(i) = state.yDot(i) = 0;
    }
}

void GraphForceSystem::update()
{
    state.next(graf);

    for (int i = 0; i < graf.nodeCount(); i++)
        graf.node(i).setPosition({ state.x(i), state.y(i) });
}

void GraphForceSystem::sendLeftButtonPressed(sf::Vector2f coords)
{
    for (int i = graf.nodeCount() - 1; i >= 0; i--)
    {
        if (graf.node(i).hitInside(coords))
        {
            dragging = true;

            draggedNodeIndex = i;
            draggedNodeInitialPos = graf.node(i).getPosition();
            draggedNodeInitialMousePos = coords;
            draggedNodeTargetPos = draggedNodeInitialPos;

            break;
        }
    }
}

void GraphForceSystem::sendLeftButtonReleased([[maybe_unused]] sf::Vector2f coords)
{
    dragging = false;
}

void GraphForceSystem::sendMouseMoved(sf::Vector2f coords)
{
    if (dragging)
        draggedNodeTargetPos = draggedNodeInitialPos + coords - draggedNodeInitialMousePos;
}

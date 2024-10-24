#include "graph_force_system.hpp"

#include "nod.hpp"
#include "utilities.hpp"
#include <SFML/System/Vector2.hpp>
#include <iostream>
#include <utility>

// sf::Vector2f GraphForceSystem::forceBetween(sf::Vector2f p1, sf::Vector2f p2)
// {
//     auto distance{Util::distance(p1, p2)};
//     auto stretch{distance - restDistance};
//     auto forceMagnitude{springConstant * stretch};

//     sf::Vector2f edgeVector{ p2.x - p1.x, p2.y - p1.y};
//     auto unitEdgeVector{Util::normalize(edgeVector)};

//     return {
//         -forceMagnitude * unitEdgeVector.x,
//         -forceMagnitude * unitEdgeVector.y
//     };
// }

GraphForceSystem::SystemState::SystemState(Graf::NoduriSSize nodeCount)
    : count{nodeCount}, values(count * 4)
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

GraphForceSystem::SystemState GraphForceSystem::SystemState::getDiffs(const Graf& graf)
{
    GraphForceSystem::SystemState diffs{count};
    
    for (int i = 0; i < count; i++) {
        diffs.x(i) = xDot(i);
        diffs.y(i) = yDot(i);

        diffs.xDot(i) = -airResistance * xDot(i);
        diffs.yDot(i) = -airResistance * yDot(i);

        for (int j = 0; j < count; j++) {
            float dist = Util::distance({x(i), y(i)}, {x(j), y(j)});
            auto distSquared{dist * dist};
            auto distAdjusted{dist + 10.f};
            auto distAdjustedSquared{distAdjusted * distAdjusted};

            sf::Vector2f directionAway{
                (x(i) - x(j)) / distAdjusted,
                (y(i) - y(j)) / distAdjusted
            };

            diffs.xDot(i) += fieldScale * directionAway.x / distAdjustedSquared;
            diffs.yDot(i) += fieldScale * directionAway.y / distAdjustedSquared;

            if (i == j || !graf.isEdge(i, j))
                continue;

            float stretch = restDistance / dist;

            auto computeDeltas = [&](float iVal, float jVal, float iValDot, float jValDot)
            {
                float delta = (iVal - jVal) * (1 - stretch);
                float deltaDot = (iValDot - jValDot) * (1 - stretch)
                    + (iVal - jVal) * stretch * (
                        (x(i) - x(j)) * xDot(i)
                        + (x(j) - x(i)) * xDot(j)
                        + (y(i) - y(j)) * yDot(i)
                        + (y(j) - y(i)) * yDot(j)
                    ) / (distSquared);

                return std::make_pair(delta, deltaDot);
            };

            auto [xDelta, xDeltaDot] = computeDeltas(x(i), x(j), xDot(i), xDot(j));
            auto [yDelta, yDeltaDot] = computeDeltas(y(i), y(j), yDot(i), yDot(j));

            diffs.xDot(i) += -springConstant * xDelta - dampingConstant * xDeltaDot;
            diffs.yDot(i) += -springConstant * yDelta - dampingConstant * yDeltaDot;
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

    auto stateK1{*this};
    auto K1{stateK1.getDiffs(graf)};
    scaleByStepSize(K1);

    auto stateK2{stateK1};
    for (int i = 0; i < count * 4; i++)
        stateK2.values[i] += K1.values[i] / 2;
    auto K2{stateK2.getDiffs(graf)};
    scaleByStepSize(K2);

    auto stateK3{stateK1};
    for (int i = 0; i < count * 4; i++)
        stateK3.values[i] += K2.values[i] / 2;
    auto K3{stateK3.getDiffs(graf)};
    scaleByStepSize(K3);

    auto stateK4{stateK1};
    for (int i = 0; i < count * 4; i++)
        stateK3.values[i] += K3.values[i];
    auto K4{stateK4.getDiffs(graf)};
    scaleByStepSize(K4);

    for (int i = 0; i < count * 4; i++)
        values[i] += (K1.values[i] + 2. * K2.values[i] + 2. * K3.values[i] + K4.values[i]) / 6.;
}

void GraphForceSystem::reload()
{
    state = SystemState{graf.nodeCount()};
    
    for (int i = 0; i < graf.nodeCount(); i++) {
        state.x(i) = graf.node(i).getPosition().x;
        state.y(i) = graf.node(i).getPosition().y;
        state.xDot(i) = state.yDot(i) = 0;
    }
}

void GraphForceSystem::update()
{
    state.next(graf);
    
    // if (dragging)
    // {
    //     auto newPos = Util::lerp(graf.node(draggedNodeIndex).getPosition(), draggedNodeTargetPos, dragSmoothness);
    //     graf.node(draggedNodeIndex).setPosition(newPos);
    // }

    for (int i = 0; i < graf.nodeCount(); i++)
        graf.node(i).setPosition({state.x(i), state.y(i)});
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

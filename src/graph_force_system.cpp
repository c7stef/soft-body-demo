#include "graph_force_system.hpp"

#include "utilities.hpp"

void GraphForceSystem::update()
{
    if (dragging)
    {
        auto newPos = Util::lerp(graf.node(draggedNodeIndex).getPosition(), draggedNodeTargetPos, dragSmoothness);
        graf.node(draggedNodeIndex).setPosition(newPos);
    }

    for (int i = 0; i < graf.nodeCount(); i++)
    {
        for (int j = i + 1; j < graf.nodeCount(); j++)
        {
            auto iPos = graf.node(i).getPosition();
            auto jPos = graf.node(j).getPosition();

            auto currentDist = Util::distance(iPos, jPos);

            auto kelasticAjustat = Util::sigmoid(kelastic * (currentDist / distNoduri));

            if (!graf.isEdge(i, j))
            {
                if (currentDist > distNoduri)
                    continue;
                kelasticAjustat = Util::sigmoid(kelastic / ((currentDist + 100) / distNoduri)) / 3.f;
            }

            auto distNoua = currentDist - kelasticAjustat * (currentDist - distNoduri);

            sf::Vector2f vectorMuchie { jPos.x - iPos.x, jPos.y - iPos.y };
            auto vectorMuchieUnitate = Util::normalize(vectorMuchie);

            sf::Vector2f vectorMuchieNou {
                vectorMuchieUnitate.x * distNoua,
                vectorMuchieUnitate.y * distNoua
            };

            sf::Vector2f deltaMuchie {
                vectorMuchie.x - vectorMuchieNou.x,
                vectorMuchie.y - vectorMuchieNou.y
            };

            sf::Vector2f iPozitieNoua {
                iPos.x + deltaMuchie.x / 2,
                iPos.y + deltaMuchie.y / 2
            };

            sf::Vector2f jPozitieNoua {
                jPos.x - deltaMuchie.x / 2,
                jPos.y - deltaMuchie.y / 2
            };

            auto applyNewPosition = [&](int index, sf::Vector2f newPos)
            {
                if (!isDragged(index))
                    graf.node(index).setPosition(newPos);
            };

            applyNewPosition(i, iPozitieNoua);
            applyNewPosition(j, jPozitieNoua);
        }
    }
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

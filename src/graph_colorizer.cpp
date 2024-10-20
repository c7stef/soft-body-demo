#include "graph_colorizer.hpp"

#include "graf.hpp"

#include <iostream>

const std::unordered_map<GraphColorizer::ColoringState, sf::Time> GraphColorizer::waitTime
{
    { ResetColors, sf::milliseconds(500) },
    { EnableColorBar, sf::milliseconds(500) },
    { SelectNextNode, sf::milliseconds(500) },
    { SelectFirstColor, sf::milliseconds(500) },
    { ApplySelectedColor, sf::milliseconds(500) },
    { DeselectNode, sf::milliseconds(500) },
    { SelectEdges, sf::milliseconds(500) },
    { DeselectEdges, sf::milliseconds(300) },
    { GenerateNextColor, sf::milliseconds(500) },
    { SelectNextColor, sf::milliseconds(800) },
};

const std::unordered_map<GraphColorizer::ColoringState, GraphColorizer::ColoringState> GraphColorizer::nextState
{
    { ResetColors, EnableColorBar },
    { EnableColorBar, SelectNextNode },
    { SelectNextNode, SelectFirstColor },
    { SelectFirstColor, CheckColorValid },
    { ApplySelectedColor, DeselectNode },
    { DeselectNode, SelectNextNode },
    { SelectEdges, DeselectEdges },
    { DeselectEdges, CheckNextColorExists },
    { GenerateNextColor, SelectNextColor },
    { SelectNextColor, CheckColorValid }
};

void GraphColorizer::start()
{
    setState(ResetColors);

    currentNode = -1;
    colors.clear();

    colorBar.close();

    for (int i = 0; i < graf.nodeCount(); i++)
    {
        graf.node(i).unhighlight();
        for (int j = 0; j < graf.nodeCount(); j++)
            if (graf.isEdge(i, j))
                graf.deselectEdge(i, j);
    }

    colorGen.reset();
}

void GraphColorizer::update()
{
    colorBar.update();

    if (currentState == ColoringStopped)
        return;

    if (currentActionDone)
    {
        if (waitTime.find(currentState) != waitTime.end())
        {
            if (stateClock.getElapsedTime() < waitTime.at(currentState))
                return;
            else
                setNextState();
        }
    }

    std::unordered_map<ColoringState, std::function<void()>> stateActions
    {
        { ResetColors, [&]()
            {
                for (auto& nod : graf.getNodes())
                    nod.resetColor();
            }
        },
        { EnableColorBar, [&]()
            {
                colorBar.reset();
                addNewColor();
            }
        },
        { SelectNextNode, [&]()
            {
                if (currentNode == graf.nodeCount() - 1)
                {
                    colorBar.close();
                    setState(ColoringStopped);
                    return;
                }

                currentNode++;
                graf.node(currentNode).highlight();
            }
        },
        { SelectFirstColor, [&]()
            {
                currentColor = 0;

                colorBar.selectFirstColor();
                graf.node(currentNode).setColor(colors[currentColor]);
            }
        },
        { CheckColorValid, [&]()
            {
                for (int i = 0; i < graf.nodeCount(); i++)
                    if (graf.isEdge(currentNode, i) && nodesHaveSameColor(currentNode, i))
                    {
                        setState(SelectEdges);
                        return;
                    }

                setState(ApplySelectedColor);
            }
        },
        { ApplySelectedColor, [&]()
            {
                colorBar.applyColor();
            }
        },
        { DeselectNode, [&]()
            {
                graf.node(currentNode).unhighlight();
            }
        },
        { SelectEdges, [&]()
            {
                for (int i = 0; i < graf.nodeCount(); i++)
                    if (graf.isEdge(currentNode, i) && nodesHaveSameColor(currentNode, i))
                        graf.selectEdge(currentNode, i);
            }
        },
        { DeselectEdges, [&]()
            {
                for (int i = 0; i < graf.nodeCount(); i++)
                    graf.deselectEdge(currentNode, i);
            }
        },
        { CheckNextColorExists, [&]()
            {
                if (reachedLastColor())
                    setState(GenerateNextColor);
                else
                    setState(SelectNextColor);
            }
        },
        { GenerateNextColor, [&]()
            {
                addNewColor();
            }
        },
        { SelectNextColor, [&]()
            {
                currentColor++;

                graf.node(currentNode).setColor(colors[currentColor]);
                colorBar.selectNextColor();
            }
        }
    };

    currentActionDone = true;
    stateActions[currentState]();

    if (currentActionDone)
        stateClock.restart();
}

void GraphColorizer::addNewColor()
{
    if (waitingForColorGen)
    {
        if (doneGeneratingColor)
        {
            auto newColor = colorGenFuture.get();

            colors.push_back(newColor);
            colorBar.addColor(newColor);

            waitingForColorGen = false;
        } else
        {
            currentActionDone = false;
            return;
        }
    } else
    {
        auto colorGenTask = [&]()
        {
            auto newColor { colorGen.getNextColor() };
            doneGeneratingColor = true;
            return newColor;
        };

        doneGeneratingColor = false;
        colorGenFuture = std::async(colorGenTask);
        waitingForColorGen = true;

        currentActionDone = false;
    }
}

bool GraphColorizer::nodesHaveSameColor(int x, int y)
{
    return graf.node(x).getColor() == graf.node(y).getColor();
}

void GraphColorizer::setState(ColoringState newState)
{
    currentState = newState;
    currentActionDone = false;
    stateClock.restart();
}

void GraphColorizer::setNextState()
{
    setState(nextState.at(currentState));
}

void GraphColorizer::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    target.draw(colorBar);
}

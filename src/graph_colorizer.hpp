#ifndef GRAPH_COLORIZER_HPP
#define GRAPH_COLORIZER_HPP

#include "color_bar.hpp"
#include "color_generator.hpp"
#include "nod.hpp"

#include <atomic>
#include <functional>
#include <future>
#include <unordered_map>
#include <vector>

class Graf;

class GraphColorizer : public sf::Drawable
{
public:
    GraphColorizer(Graf& graph) :
        graf { graph }
    {
    }

    void start();
    void update();

    bool isStopped() { return currentState == ColoringStopped; }

private:
    Graf& graf;

    enum ColoringState
    {
        ColoringStopped,
        ResetColors,
        EnableColorBar,
        SelectNextNode,
        SelectFirstColor,
        CheckColorValid,
        ApplySelectedColor,
        DeselectNode,
        SelectEdges,
        DeselectEdges,
        CheckNextColorExists,
        GenerateNextColor,
        SelectNextColor
    };

    ColoringState currentState { ColoringStopped };
    bool currentActionDone { true };
    sf::Clock stateClock {};

    ColorBar colorBar {};
    ColorGenerator colorGen {};

    int currentNode { -1 };
    int currentColor { -1 };

    std::vector<sf::Color> colors {};

    bool nodesHaveSameColor(int x, int y);

    bool waitingForColorGen { false };
    std::atomic<bool> doneGeneratingColor {};

    std::future<sf::Color> colorGenFuture {};

    void addNewColor();

    int colorCount() { return static_cast<int>(colors.size()); }
    bool reachedLastColor() { return currentColor == colorCount() - 1; }

    static const std::unordered_map<ColoringState, sf::Time> waitTime;
    static const std::unordered_map<ColoringState, ColoringState> nextState;

    void setState(ColoringState newState);
    void setNextState();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // GRAPH_COLORIZER_HPP

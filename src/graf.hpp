#ifndef GRAF_HPP
#define GRAF_HPP

#include "nod.hpp"
#include <numeric>

#include <SFML/Graphics.hpp>

#include <string>
#include <unordered_map>

class Graf : public sf::Drawable
{
public:
    Graf() {}

    using NodeList = std::vector<Nod>;
    using AdjacencyMatrix = std::vector<std::vector<int>>;

private:
    NodeList noduri {};

public:
    using NoduriSSize = std::make_signed<decltype(noduri)::size_type>::type;

    void update();

    void loadFromFile(std::string filename);
    void openFileDialogAndLoad();

    NodeList& getNodes() { return noduri; }

    const Nod& node(int index) const { return noduri[index]; }
    Nod& node(int index) { return noduri[index]; }
    bool isEdge(int x, int y) const { return adiacenta[x][y]; }

    void selectEdge(int x, int y);
    void deselectEdge(int x, int y);

    NoduriSSize nodeCount() const { return static_cast<NoduriSSize>(noduri.size()); }

private:
    AdjacencyMatrix adiacenta {};

    class EdgeInfo
    {
    public:
        void update();

        void highlight();
        void unhighlight();

        auto thickness() const { return currentThickness; }
        auto color() const { return currentColor; }

    private:
        sf::Color targetColor { 0, 0, 0, 40 };
        sf::Color currentColor { targetColor };
        float targetThickness { 4.0f };
        float currentThickness { targetThickness };

        static constexpr float smoothness { 0.16f };
    };

    std::unordered_map<int, std::unordered_map<int, EdgeInfo>> edgeInfo;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // GRAF_HPP

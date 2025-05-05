#ifndef GRAF_HPP
#define GRAF_HPP

#include "object.hpp"
#include "nod.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <unordered_map>
#include <unordered_set>

class Mesh : public Object
{
public:
    Mesh(const sf::Font& font)
        : font{font}
    {
    }

    using NodeList = std::vector<Nod>;
    using AdjacencyMatrix = std::unordered_map<int, std::unordered_map<int, float>>;

private:
    NodeList noduri {};

public:
    using NoduriSSize = std::make_signed<decltype(noduri)::size_type>::type;

    void update(float deltaTime) override;

    void loadRawAdjacency(std::string filename);
    void loadFromFile(std::string filename, float resolution);
    void openFileDialogAndLoad(float resolution);

    NodeList& getNodes() { return noduri; }

    const Nod& node(int index) const { return noduri[index]; }
    Nod& node(int index) { return noduri[index]; }
    bool isEdge(int x, int y) const { return adiacenta.at(x).contains(y); }
    float edgeLength(int x, int y) const;

    void selectEdge(int x, int y);
    void deselectEdge(int x, int y);

    NoduriSSize nodeCount() const { return static_cast<NoduriSSize>(noduri.size()); }

    void sendKeyPressed(sf::Keyboard::Key key) override;

private:
    AdjacencyMatrix adiacenta {};
    const sf::Font& font;

    sf::Texture image {};
    std::vector<sf::Vector2f> controlPoints {};

    static constexpr float pointDensity = 0.8f;
    static constexpr float scale = 0.6f;
    static constexpr float meshImageSpacing = 10.f;

    bool showImage { false };

    class EdgeInfo
    {
    public:
        void update([[maybe_unused]] float deltaTime);

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

    struct TriangleInfo
    {
        int a{};
        int b{};
        int c{};
        float restSignedArea{};
    };

    std::vector<TriangleInfo> triangleInfo{};

public:
    std::vector<TriangleInfo> const& triangles() const
    {
        return triangleInfo;
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // GRAF_HPP

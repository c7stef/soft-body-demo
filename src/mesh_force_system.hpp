#ifndef GRAPH_FORCE_SYSTEM_HPP
#define GRAPH_FORCE_SYSTEM_HPP

#include "mesh.hpp"

#include <SFML/Graphics.hpp>

#include <functional>
#include <memory>
#include <unordered_set>

class Mesh;

class MeshForceSystem : public Object
{
public:
    MeshForceSystem(std::weak_ptr<Mesh> mesh) :
        mesh { mesh }
    {
    }

    void update([[maybe_unused]] float deltaTime) override;
    void reload();

    void sendLeftButtonPressed(sf::Vector2f coords);
    void sendRightButtonPressed(sf::Vector2f coords);
    void sendLeftButtonReleased(sf::Vector2f coords);
    void sendMouseMoved(sf::Vector2f coords);
    void sendKeyPressed(sf::Keyboard::Key key);

    float getMomentum();
    float getAngularMomentum();

private:
    std::weak_ptr<Mesh> mesh;

    bool gravity { false };
    static constexpr float gravityStrength = 5e3f;
    static constexpr float groundLevel = 700.f;

    bool dragging { false };
    int draggedNode { -1 };

    std::unordered_set<int> fixedNodes{};

    sf::Vector2f mousePos{};

    int getClosestNodeTo(sf::Vector2f coords) const;

    static constexpr float springConstant = 3e4f;
    static constexpr float dampingConstant = 300.0f;
    static constexpr float airResistance = 0.f;
    static constexpr float fieldScale = 5e6f;
    static constexpr float areaSpringConstant = 700.f;
    static constexpr float areaDampingConstant = 0.f;

    static constexpr float stepSize = 0.00033f;

    class SystemState
    {
    public:
        SystemState(Mesh::NoduriSSize count, MeshForceSystem const& forceSystem);

        float& x(int index);
        float& y(int index);
        float& xDot(int index);
        float& yDot(int index);

        void next(Mesh const& mesh);

    private:
        SystemState getDiffs(Mesh const& mesh);

        Mesh::NoduriSSize count;
        std::vector<float> values;

        std::reference_wrapper<MeshForceSystem const> forceSystem;
    };

    SystemState state{mesh.lock()->nodeCount(), *this};

    friend SystemState;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // GRAPH_FORCE_SYSTEM_HPP

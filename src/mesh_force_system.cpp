#include "mesh_force_system.hpp"

#include "nod.hpp"
#include "utilities.hpp"
#include <SFML/System/Vector2.hpp>
#include <utility>
#include <limits>
#include <iostream>

MeshForceSystem::SystemState::SystemState(Mesh::NoduriSSize nodeCount, const MeshForceSystem& forceSystem)
    : count{nodeCount}, values(count * 4), forceSystem{forceSystem}
{
}

float& MeshForceSystem::SystemState::x(int index)
{
    return values[index * 4];
}

float& MeshForceSystem::SystemState::y(int index)
{
    return values[index * 4 + 1];
}

float& MeshForceSystem::SystemState::xDot(int index)
{
    return values[index * 4 + 2];
}

float& MeshForceSystem::SystemState::yDot(int index)
{
    return values[index * 4 + 3];
}

static float distanceAdjusted(sf::Vector2f a, sf::Vector2f b)
{
    constexpr float offset = 0.01f;
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

    sf::Vector2f n{
        (targetPos.x - sourcePos.x) / dist,
        (targetPos.y - sourcePos.y) / dist
    };

    float displacement = dist - restDistance;
    float vdotn = (targetVelocity.x - sourceVelocity.x) * n.x + (targetVelocity.y - sourceVelocity.y) * n.y;

    return (-springConstant * displacement - dampingConstant * vdotn) * n;
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

MeshForceSystem::SystemState MeshForceSystem::SystemState::getDiffs(const Mesh& mesh)
{
    MeshForceSystem::SystemState diffs { count, forceSystem };
    
    for (int i = 0; i < count; i++) {
        if (forceSystem.get().fixedNodes.contains(i)) {
            diffs.xDot(i) = 0;
            diffs.yDot(i) = 0;
            continue;
        }

        diffs.x(i) = xDot(i);
        diffs.y(i) = yDot(i);

        diffs.xDot(i) = -airResistance * xDot(i);
        diffs.yDot(i) = -airResistance * yDot(i);

        if (forceSystem.get().gravity) {
            diffs.yDot(i) += gravityStrength;

            auto electrostatic { electrostaticForce(
                { x(i), y(i) },
                { x(i), groundLevel },
                fieldScale
            ) };

            diffs.yDot(i) += electrostatic.y;
        }


        for (int j = 0; j < count; j++) {

            if (i == j || !mesh.isEdge(i, j))
                continue;

            auto spring { springForce(
                { x(i), y(i) },
                { xDot(i), yDot(i) },
                { x(j), y(j) },
                { xDot(j), yDot(j) },
                mesh.edgeLength(i, j), springConstant, dampingConstant
            )};

            auto fixedCoef = forceSystem.get().fixedNodes.contains(j) ? 2.f : 1.f;

            diffs.xDot(i) += spring.x * fixedCoef;
            diffs.yDot(i) += spring.y * fixedCoef;
        }
    }

    if (forceSystem.get().dragging) {
        auto mousePos { forceSystem.get().mousePos };
        auto draggedNode { forceSystem.get().draggedNode };

        if (draggedNode != -1) {
            auto mouseForce { springForce(
                { x(draggedNode), y(draggedNode) },
                { xDot(draggedNode), yDot(draggedNode) },
                forceSystem.get().mousePos,
                { 0.f, 0.f },
                0.f,
                springConstant * 2,
                dampingConstant
            ) };

            diffs.xDot(draggedNode) += mouseForce.x;
            diffs.yDot(draggedNode) += mouseForce.y;
        }
    }
    
    for (auto const& tri : mesh.triangles()) {
        float areaDiff = Util::signedArea(
            {x(tri.a), y(tri.a)},
            {x(tri.b), y(tri.b)},
            {x(tri.c), y(tri.c)}
        ) - tri.restSignedArea;

        auto gradient_a = 0.5f * sf::Vector2f{y(tri.b) - y(tri.c), x(tri.c) - x(tri.b)};
        auto gradient_b = 0.5f * sf::Vector2f{y(tri.c) - y(tri.a), x(tri.a) - x(tri.c)};
        auto gradient_c = 0.5f * sf::Vector2f{y(tri.a) - y(tri.b), x(tri.b) - x(tri.a)};

        auto forceCoef = areaSpringConstant * areaDiff;
        
        diffs.xDot(tri.a) += -forceCoef * gradient_a.x;
        diffs.xDot(tri.b) += -forceCoef * gradient_b.x;
        diffs.xDot(tri.c) += -forceCoef * gradient_c.x;

        diffs.yDot(tri.a) += -forceCoef * gradient_a.y;
        diffs.yDot(tri.b) += -forceCoef * gradient_b.y;
        diffs.yDot(tri.c) += -forceCoef * gradient_c.y;
    }

    return diffs;
}

float MeshForceSystem::getMomentum()
{
    sf::Vector2f momentum { 0.f, 0.f };
    auto nodeCount { mesh.lock()->nodeCount() };
    for (int i = 0; i < nodeCount; i++) {
        momentum.x += state.xDot(i);
        momentum.y += state.yDot(i);
    }
    return Util::distance(momentum, { 0.f, 0.f });
}

float MeshForceSystem::getAngularMomentum()
{
    float angularMomentum { 0.f };
    auto nodeCount { mesh.lock()->nodeCount() };
    for (int i = 0; i < nodeCount; i++) {
        angularMomentum += state.x(i) * state.yDot(i) - state.y(i) * state.xDot(i);
    }
    return angularMomentum;
}

void MeshForceSystem::SystemState::next(const Mesh& mesh)
{
    auto scaleByStepSize = [&](auto& state) {
        for (int i = 0; i < count * 4; i++)
            state.values[i] *= stepSize;
    };

    auto stateK1 { *this };
    auto K1 { stateK1.getDiffs(mesh) };
    scaleByStepSize(K1);

    auto stateK2 { stateK1 };
    for (int i = 0; i < count * 4; i++)
        stateK2.values[i] += K1.values[i] / 2;
    auto K2 { stateK2.getDiffs(mesh) };
    scaleByStepSize(K2);

    auto stateK3 { stateK1 };
    for (int i = 0; i < count * 4; i++)
        stateK3.values[i] += K2.values[i] / 2;
    auto K3 { stateK3.getDiffs(mesh) };
    scaleByStepSize(K3);

    auto stateK4 { stateK1 };
    for (int i = 0; i < count * 4; i++)
        stateK3.values[i] += K3.values[i];
    auto K4 { stateK4.getDiffs(mesh) };
    scaleByStepSize(K4);

    for (int i = 0; i < count * 4; i++)
        values[i] += (K1.values[i] + 2. * K2.values[i] + 2. * K3.values[i] + K4.values[i]) / 6.;
}

void MeshForceSystem::reload()
{
    state = SystemState { mesh.lock()->nodeCount(), *this };
    
    for (int i = 0; i < mesh.lock()->nodeCount(); i++) {
        state.x(i) = mesh.lock()->node(i).getPosition().x;
        state.y(i) = mesh.lock()->node(i).getPosition().y;
        state.xDot(i) = state.yDot(i) = 0.f;
    }
}

void MeshForceSystem::update([[maybe_unused]] float deltaTime)
{
    for (int i = 0; i < 100; i++)
        state.next(*mesh.lock());

    for (int i = 0; i < mesh.lock()->nodeCount(); i++)
        mesh.lock()->node(i).setPosition({ state.x(i), state.y(i) });
}

int MeshForceSystem::getClosestNodeTo(sf::Vector2f coords) const
{
    int closestToMouse { -1 };
    float closestDistance { std::numeric_limits<float>::max() };

    for (int i = 0; i < mesh.lock()->nodeCount(); i++)
        if (Util::distance(mesh.lock()->node(i).getPosition(), coords) < closestDistance) {
            closestDistance = Util::distance(mesh.lock()->node(i).getPosition(), coords);
            closestToMouse = i;
        }

    return closestToMouse;
}

void MeshForceSystem::sendLeftButtonPressed(sf::Vector2f coords)
{
    dragging = true;
    mousePos = coords;

    draggedNode = getClosestNodeTo(coords);

    if (draggedNode != -1) {
        auto& node { mesh.lock()->node(draggedNode) };
        node.highlight();
    }
}

void MeshForceSystem::sendLeftButtonReleased([[maybe_unused]] sf::Vector2f coords)
{
    dragging = false;
    if (draggedNode == -1)
        return;

    mesh.lock()->node(draggedNode).unhighlight();
    draggedNode = -1;
}

void MeshForceSystem::sendRightButtonPressed([[maybe_unused]] sf::Vector2f coords)
{
    auto closestNode { getClosestNodeTo(coords) };
    if (closestNode != -1) {
        if (fixedNodes.contains(closestNode)) {
            fixedNodes.erase(closestNode);
            mesh.lock()->node(closestNode).resetColor();
        } else {
            fixedNodes.insert(closestNode);
            mesh.lock()->node(closestNode).setColor({ 200, 0, 200, 200 });
        }
    }
}

void MeshForceSystem::sendMouseMoved(sf::Vector2f coords)
{
    mousePos = coords;
}

void MeshForceSystem::sendKeyPressed(sf::Keyboard::Key key)
{
    if (key == sf::Keyboard::G) {
        gravity = !gravity;
    }
}

void MeshForceSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    if (gravity) {
        // draw ground as a rectangle
        sf::RectangleShape ground { { 1000.f, 10.f } };
        ground.setPosition(0.f, groundLevel);
        ground.setFillColor({ 0, 0, 0, 100 });
        target.draw(ground, states);
    }
}

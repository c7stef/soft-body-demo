#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <SFML/Graphics.hpp>

class Object : public sf::Drawable
{
public:
    Object() = default;

    virtual void update([[maybe_unused]] float deltaTime) {};
    virtual void draw(
        [[maybe_unused]] sf::RenderTarget& target,
        [[maybe_unused]] sf::RenderStates states
    ) const override {};

    virtual void sendLeftButtonPressed([[maybe_unused]] sf::Vector2f coords) {}
    virtual void sendRightButtonPressed([[maybe_unused]] sf::Vector2f coords) {}
    virtual void sendLeftButtonReleased([[maybe_unused]] sf::Vector2f coords) {}
    virtual void sendMouseMoved([[maybe_unused]] sf::Vector2f coords) {}
    virtual void sendKeyPressed([[maybe_unused]] sf::Keyboard::Key key) {}
};

#endif // OBJECT_HPP
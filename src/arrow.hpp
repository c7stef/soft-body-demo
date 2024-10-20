#ifndef ARROW_HPP
#define ARROW_HPP

#include <SFML/Graphics.hpp>

class Arrow : public sf::Drawable
{
public:
    Arrow() {}

    void setPositionInstant(sf::Vector2f newPos) { currentPosition = targetPosition = newPos; }
    void setPosition(sf::Vector2f newPos) { targetPosition = newPos; }
    sf::Vector2f getPosition() { return targetPosition; }

    void setColorInstant(sf::Color newColor) { currentColor = targetColor = newColor; }
    void setColor(sf::Color newColor) { targetColor = newColor; }
    sf::Color getColor() { return targetColor; }

    void update();

    bool isTransparent();

private:
    sf::Vector2f targetPosition {};
    sf::Vector2f currentPosition { targetPosition };

    sf::Color targetColor { sf::Color::White };
    sf::Color currentColor { targetColor };

    static constexpr float smoothness { 0.15f };

    static const sf::Vector2f headSize;
    static const sf::Vector2f tailSize;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // ARROW_HPP

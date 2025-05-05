#ifndef NOD_HPP
#define NOD_HPP

#include "fcolor.hpp"
#include "object.hpp"

#include <SFML/Graphics.hpp>

#include <string>

class Nod : public Object
{
public:
    Nod(const std::string& content, const sf::Font& font);
    Nod(int value, const sf::Font& font);

    void setPosition(const sf::Vector2f& newPos);
    void setPosition(float x, float y) { setPosition({x, y}); }

    void setColor(sf::Color newColor);
    void resetColor();

    sf::Color getColor() { return targetColor; }

    void highlight();
    void unhighlight();

    void update([[maybe_unused]] float deltaTime) override;

    sf::Vector2f getPosition() const { return position; }

    bool hitInside(sf::Vector2f coords) const;

private:
    sf::CircleShape backgroundCircle { circleRadius };

    sf::Vector2f position {};

    fColor targetColor { 20, 159, 219 };
    fColor targetOutlineColor { sf::Color::Transparent };
    float targetOutlineThickness { 0 };

    bool colorReset = false;

    fColor currentColor { targetColor };
    fColor currentOutlineColor { targetOutlineColor };
    float currentOutlineThickness { targetOutlineThickness };

    static constexpr unsigned fontSize { 26 };
    static constexpr float circleRadius { 4.f };

    static constexpr float transitionSmoothness { 0.1f };

    static const fColor defaultHighlightColor;
    static constexpr float highlightTransparency { 100 };

    void adjustOutlineColor();

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // NOD_HPP

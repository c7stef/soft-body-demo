#include "nod.hpp"

#include "background.hpp"
#include "utilities.hpp"

const fColor Nod::defaultHighlightColor { 38, 52, 79, 80 };

Nod::Nod(const std::string& content, const sf::Font& font)
{
    backgroundCircle.setFillColor(currentColor);

    setPosition(0, 0);
}

Nod::Nod(int value, const sf::Font& font) :
    Nod { std::to_string(value), font }
{
}

void Nod::setPosition(const sf::Vector2f& newPos)
{
    position = newPos;

    auto radius = backgroundCircle.getRadius();

    backgroundCircle.setPosition(
        position.x - radius,
        position.y - radius
    );
}

void Nod::setColor(sf::Color newColor)
{
    colorReset = false;

    targetColor = newColor;
    adjustOutlineColor();
}

void Nod::resetColor()
{
    setColor(Background::bgColor);

    colorReset = true;
}

void Nod::highlight()
{
    targetOutlineThickness = 4;
    adjustOutlineColor();
}

void Nod::unhighlight()
{
    targetOutlineThickness = 0;
}

void Nod::update([[maybe_unused]] float deltaTime)
{
    currentColor = Util::lerp(currentColor, targetColor, transitionSmoothness);
    currentOutlineColor = Util::lerp(currentOutlineColor, targetOutlineColor, transitionSmoothness);
    currentOutlineThickness = Util::lerp(currentOutlineThickness, targetOutlineThickness, transitionSmoothness);

    backgroundCircle.setFillColor(currentColor);
    backgroundCircle.setOutlineColor(currentOutlineColor);
    backgroundCircle.setOutlineThickness(currentOutlineThickness);
}

bool Nod::hitInside(sf::Vector2f coords) const
{
    return Util::distance(position, coords) < circleRadius;
}

void Nod::adjustOutlineColor()
{
    if (colorReset)
        targetOutlineColor = defaultHighlightColor;
    else
    {
        targetOutlineColor = targetColor;
        targetOutlineColor.a = highlightTransparency;
    }
}

void Nod::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    target.draw(backgroundCircle);
    // target.draw(displayText);
}

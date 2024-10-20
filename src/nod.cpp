#include "nod.hpp"

#include "background.hpp"
#include "fonts.hpp"
#include "utilities.hpp"

#include <iostream>

const fColor Nod::textColorDark { 0, 0, 0, 200 };
const fColor Nod::textColorLight { 255, 255, 255 };

const fColor Nod::defaultHighlightColor { 38, 52, 79, 80 };

Nod::Nod(const std::string& content) :
    displayText { content, Fonts::numberFont }
{
    backgroundCircle.setFillColor(currentColor);

    displayText.setCharacterSize(fontSize);
    displayText.setFillColor(currentTextColor);

    setPosition(0, 0);
}

Nod::Nod(int value) :
    Nod { std::to_string(value) }
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

    auto textBounds = displayText.getGlobalBounds();

    // Pozitia este ajustata cu { -2, -fontSize / 4.f } fiindca
    // corecteaza aproximativ imprecizia functiei getGlobalBounds()
    displayText.setPosition(
        position.x - textBounds.width / 2 - 2,
        position.y - textBounds.height / 2 - fontSize / 4.f
    );
}

void Nod::setColor(sf::Color newColor)
{
    colorReset = false;

    targetColor = newColor;
    adjustTextColor();
    adjustOutlineColor();
}

void Nod::resetColor()
{
    setColor(Background::bgColor);

    colorReset = true;
}

void Nod::highlight()
{
    targetOutlineThickness = 6;
    adjustOutlineColor();
}

void Nod::unhighlight()
{
    targetOutlineThickness = 0;
}

void Nod::update()
{
    currentColor = Util::lerp(currentColor, targetColor, transitionSmoothness);
    currentTextColor = Util::lerp(currentTextColor, targetTextColor, transitionSmoothness);
    currentOutlineColor = Util::lerp(currentOutlineColor, targetOutlineColor, transitionSmoothness);
    currentOutlineThickness = Util::lerp(currentOutlineThickness, targetOutlineThickness, transitionSmoothness);

    backgroundCircle.setFillColor(currentColor);
    backgroundCircle.setOutlineColor(currentOutlineColor);
    backgroundCircle.setOutlineThickness(currentOutlineThickness);

    displayText.setFillColor(currentTextColor);
}

bool Nod::hitInside(sf::Vector2f coords) const
{
    return Util::distanta(position, coords) < circleRadius;
}

void Nod::adjustTextColor()
{
    targetTextColor = Util::computeLuma(targetColor) > darkTextLumaThreshold * Util::maxLumaValue
        ? textColorDark
        : textColorLight;
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
    target.draw(displayText);
}

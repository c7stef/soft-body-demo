#include "button.hpp"

#include "background.hpp"
#include "fonts.hpp"
#include "utilities.hpp"

Button::Button(std::string text, std::function<void()> action) :
    displayText { text, Fonts::textFont },
    callback { action }
{
    displayText.setCharacterSize(fontSize);

    setPosition({});
    setStyle(Primary);
}

void Button::update()
{

}

void Button::setPosition(sf::Vector2f newPos)
{
    position = newPos;

    displayText.setPosition(newPos + padding - sf::Vector2f { 2, 4 });
}

void Button::setStyle(Style newStyle)
{
    style = newStyle;
    updateTextColor();
}

sf::Vector2f Button::getBounds()
{
    auto textBounds = displayText.getGlobalBounds();

    return {
        textBounds.width + padding.x * 2.0f,
        textBounds.height + padding.y * 2.0f
    };
}

void Button::updateTextColor()
{
    auto baseColor { style == Primary ? secondaryColor : primaryColor };

    if (enabled)
        displayText.setFillColor(baseColor);
    else
        displayText.setFillColor(Util::lerp(baseColor, Background::bgColor, 0.75));
}

void Button::sendLeftButtonPressed(sf::Vector2f coords)
{
    mousePressed = hitInside(coords);
}

void Button::sendLeftButtonReleased([[maybe_unused]] sf::Vector2f coords)
{
    if (mousePressed && enabled)
        callback();

    mousePressed = false;
}

void Button::sendMouseMoved(sf::Vector2f coords)
{
    mouseIn = hitInside(coords);

    if (!mouseIn)
        mousePressed = false;
}

bool Button::hitInside(sf::Vector2f coords) const
{
    auto textBounds = displayText.getGlobalBounds();

    sf::Vector2f bottomRight {
        position.x + padding.x * 2.0f + textBounds.width,
        position.y + padding.y * 2.0f + textBounds.height
    };

    return coords.x >= position.x
        && coords.y >= position.y
        && coords.x <= bottomRight.x
        && coords.y <= bottomRight.y;
}

void Button::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    auto textBounds = displayText.getGlobalBounds();

    sf::Vector2f circleOffset {
        padding.x * 2.0f + textBounds.width - borderRadius * 2.0f,
        padding.y * 2.0f + textBounds.height - borderRadius * 2.0f
    };

    sf::CircleShape topLeftCircle { borderRadius };
    topLeftCircle.setPosition(position);

    sf::CircleShape topRightCircle { borderRadius };
    topRightCircle.setPosition(position + sf::Vector2f { circleOffset.x, 0 });

    sf::CircleShape bottomLeftCircle { borderRadius };
    bottomLeftCircle.setPosition(position + sf::Vector2f { 0, circleOffset.y });

    sf::CircleShape bottomRightCircle { borderRadius };
    bottomRightCircle.setPosition(position + sf::Vector2f { circleOffset.x, circleOffset.y });

    sf::RectangleShape bgRectVertical {
        sf::Vector2f {
            textBounds.width - borderRadius * 2.0f,
            textBounds.height
        } + padding * 2.0f
    };

    bgRectVertical.setPosition(position + sf::Vector2f { borderRadius, 0 });

    sf::RectangleShape bgRectHorizontal {
        sf::Vector2f {
            textBounds.width,
            textBounds.height - borderRadius * 2.0f
        } + padding * 2.0f
    };

    bgRectHorizontal.setPosition(position + sf::Vector2f { 0, borderRadius });

    sf::Color primaryColorAdjusted = primaryColor;
    sf::Color secondaryColorAdjusted = secondaryColor;

    auto adjustColorDisabled = [&](sf::Color& targetColor)
    {
        targetColor = Util::lerp(targetColor, Background::bgColor, 0.6f);
    };

    auto adjustColor = [&](sf::Color& targetColor, sf::Color adjustmentColor, float hoverRatio, float pressRatio)
    {
        if (!enabled)
            adjustColorDisabled(targetColor);
        else if (mousePressed)
            targetColor = Util::lerp(targetColor, adjustmentColor, pressRatio);
        else if (mouseIn)
            targetColor = Util::lerp(targetColor, adjustmentColor, hoverRatio);
    };

    if (style == Primary)
    {
        adjustColor(primaryColorAdjusted, sf::Color::Black, 0.1f, 0.2f);

        if (!enabled)
            adjustColorDisabled(secondaryColorAdjusted);

    } else
    {
        adjustColor(secondaryColorAdjusted, primaryColor, 0.1f, 0.2f);

        if (!enabled)
            adjustColorDisabled(primaryColorAdjusted);
    }

    if (style == Primary)
    {
        topLeftCircle.setFillColor(primaryColorAdjusted);
        topRightCircle.setFillColor(primaryColorAdjusted);
        bottomLeftCircle.setFillColor(primaryColorAdjusted);
        bottomRightCircle.setFillColor(primaryColorAdjusted);

        bgRectVertical.setFillColor(primaryColorAdjusted);
        bgRectHorizontal.setFillColor(primaryColorAdjusted);
    } else
    {
        topLeftCircle.setRadius(borderRadius - outlineWidth);
        topRightCircle.setRadius(borderRadius - outlineWidth);
        bottomLeftCircle.setRadius(borderRadius - outlineWidth);
        bottomRightCircle.setRadius(borderRadius - outlineWidth);

        sf::Vector2f outlineOffset { outlineWidth, outlineWidth };

        topLeftCircle.setPosition(topLeftCircle.getPosition() + outlineOffset);
        topRightCircle.setPosition(topRightCircle.getPosition() + outlineOffset);
        bottomLeftCircle.setPosition(bottomLeftCircle.getPosition() + outlineOffset);
        bottomRightCircle.setPosition(bottomRightCircle.getPosition() + outlineOffset);

        topLeftCircle.setFillColor(secondaryColorAdjusted);
        topRightCircle.setFillColor(secondaryColorAdjusted);
        bottomLeftCircle.setFillColor(secondaryColorAdjusted);
        bottomRightCircle.setFillColor(secondaryColorAdjusted);

        topLeftCircle.setOutlineColor(primaryColorAdjusted);
        topRightCircle.setOutlineColor(primaryColorAdjusted);
        bottomLeftCircle.setOutlineColor(primaryColorAdjusted);
        bottomRightCircle.setOutlineColor(primaryColorAdjusted);

        topLeftCircle.setOutlineThickness(outlineWidth);
        topRightCircle.setOutlineThickness(outlineWidth);
        bottomLeftCircle.setOutlineThickness(outlineWidth);
        bottomRightCircle.setOutlineThickness(outlineWidth);

        bgRectVertical.setFillColor(secondaryColorAdjusted);
        bgRectHorizontal.setFillColor(secondaryColorAdjusted);
    }

    target.draw(topLeftCircle);
    target.draw(topRightCircle);
    target.draw(bottomLeftCircle);
    target.draw(bottomRightCircle);

    target.draw(bgRectVertical);
    target.draw(bgRectHorizontal);

    if (style == Secondary)
    {
        sf::RectangleShape leftLine {
            sf::Vector2f { outlineWidth, textBounds.height + padding.y * 2.0f - borderRadius * 2.0f }
        };

        sf::RectangleShape topLine {
            sf::Vector2f { textBounds.width + padding.x * 2.0f - borderRadius * 2.0f, outlineWidth }
        };

        sf::RectangleShape rightLine {
            sf::Vector2f { outlineWidth, textBounds.height + padding.y * 2.0f - borderRadius * 2.0f }
        };

        sf::RectangleShape bottomLine {
            sf::Vector2f { textBounds.width + padding.x * 2.0f - borderRadius * 2.0f, outlineWidth }
        };

        leftLine.setPosition(position + sf::Vector2f { 0, borderRadius });
        topLine.setPosition(position + sf::Vector2f { borderRadius, 0 });
        rightLine.setPosition(position + sf::Vector2f { textBounds.width + padding.x * 2.0f - outlineWidth, borderRadius });
        bottomLine.setPosition(position + sf::Vector2f { borderRadius, textBounds.height + padding.y * 2.0f - outlineWidth });

        leftLine.setFillColor(primaryColorAdjusted);
        topLine.setFillColor(primaryColorAdjusted);
        rightLine.setFillColor(primaryColorAdjusted);
        bottomLine.setFillColor(primaryColorAdjusted);

        target.draw(leftLine);
        target.draw(topLine);
        target.draw(rightLine);
        target.draw(bottomLine);
    }

    target.draw(displayText);
}

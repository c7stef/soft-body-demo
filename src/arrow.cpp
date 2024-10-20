#include "arrow.hpp"

#include "utilities.hpp"

#include <cmath>

const sf::Vector2f Arrow::headSize { 50, 60 };
const sf::Vector2f Arrow::tailSize { 40, 30 };

void Arrow::update()
{
    currentPosition = Util::lerp(currentPosition, targetPosition, smoothness);
    currentColor = Util::lerp(currentColor, targetColor, smoothness);
}

bool Arrow::isTransparent()
{
    return currentColor.a < 1.0f;
}

void Arrow::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    // Set up arrow head as a triangle
    sf::ConvexShape headTriangle {};
    headTriangle.setPointCount(3);

    headTriangle.setPoint(0, currentPosition);
    headTriangle.setPoint(1, currentPosition + sf::Vector2f { -headSize.x, -headSize.y / 2 });
    headTriangle.setPoint(2, currentPosition + sf::Vector2f { -headSize.x, headSize.y / 2 });

    headTriangle.setFillColor(currentColor);

    // Set up arrow tail as a rectangle
    sf::RectangleShape tailRect { tailSize };
    tailRect.setPosition(currentPosition + sf::Vector2f { -(headSize.x + tailSize.x), -tailSize.y / 2 });

    tailRect.setFillColor(currentColor);

    // Render arrow to target
    target.draw(headTriangle);
    target.draw(tailRect);
}

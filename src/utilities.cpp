#include "utilities.hpp"

#include <cmath>

float Util::distance(sf::Vector2f a, sf::Vector2f b)
{
    return std::sqrt(
        (b.x - a.x) * (b.x - a.x) +
        (b.y - a.y) * (b.y - a.y)
    );
}

sf::Vector2f Util::normalize(sf::Vector2f v)
{
    auto dist = Util::distance({0, 0}, v);
    return { v.x / dist, v.y / dist };
}

float Util::sigmoid(float x)
{
    return (1 / (1 + std::exp(-x)))*2-1;
}

float Util::lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}

sf::Vector2f Util::lerp(sf::Vector2f a, sf::Vector2f b, float t)
{
    return a + (b - a) * t;
}

sf::Vector3f Util::lerp(sf::Vector3f a, sf::Vector3f b, float t)
{
    return a + (b - a) * t;
}

fColor Util::lerp(fColor a, fColor b, float t)
{
    return a + (b - a) * t;
}

float Util::computeLuma(sf::Color color)
{
    return computeLuma(fColor { color });
}

float Util::computeLuma(fColor color)
{
    // Formula standard pentru "luma" (relative luminance)
    // Mai multe informatii aici:
    // https://wikipedia.org/wiki/Relative_luminance/

    return 0.2126f * color.r + 0.7152f * color.g + 0.0722 * color.b;
}

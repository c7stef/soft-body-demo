#ifndef UTIL_HPP
#define UTIL_HPP

#include "fcolor.hpp"

#include <SFML/Graphics.hpp>

namespace Util
{
    float distance(sf::Vector2f a, sf::Vector2f b);
    sf::Vector2f normalize(sf::Vector2f v);
    float signedArea(const sf::Vector2f& a, const sf::Vector2f b, const sf::Vector2f c);

    float sigmoid(float x);

    float lerp(float a, float b, float t);
    sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, float t);
    sf::Vector3f lerp(sf::Vector3f a, sf::Vector3f b, float t);
    fColor lerp(fColor a, fColor b, float t);

    float computeLuma(sf::Color color);
    float computeLuma(fColor color);

    constexpr float maxLumaValue { 255.f };

    const sf::Vector2u windowSize { 1000, 800 };
}

#endif // UTIL_HPP

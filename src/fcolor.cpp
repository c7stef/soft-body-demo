#include "fcolor.hpp"

fColor::operator sf::Color() const
{
    auto toUint = [](float value) { return static_cast<sf::Uint8>(value); };

    return {
        toUint(r),
        toUint(g),
        toUint(b),
        toUint(a)
    };
}

bool operator==(const fColor& left, const fColor& right)
{
    return left.r == right.r
        && left.g == right.g
        && left.b == right.b
        && left.a == right.a;
}

bool operator!=(const fColor& left, const fColor& right)
{
    return !(left == right);
}

fColor operator+(const fColor& left, const fColor& right)
{
    return {
        left.r + right.r,
        left.g + right.g,
        left.b + right.b,
        left.a + right.a
    };
}

fColor operator*(const fColor& left, float right)
{
    return {
        left.r * right,
        left.g * right,
        left.b * right,
        left.a * right
    };
}

fColor operator-(const fColor& left, const fColor& right)
{
    return {
        left.r - right.r,
        left.g - right.g,
        left.b - right.b,
        left.a - right.a
    };
}

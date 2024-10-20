#ifndef FCOLOR_HPP
#define FCOLOR_HPP

#include <SFML/Graphics.hpp>

struct fColor
{
    fColor() {}

    fColor(float red, float green, float blue, float alpha = 255) :
        r { red }, g { green }, b { blue }, a { alpha }
    {
    }

    fColor(sf::Color color) :
        r { static_cast<float>(color.r) },
        g { static_cast<float>(color.g) },
        b { static_cast<float>(color.b) },
        a { static_cast<float>(color.a) }
    {
    }

    float r {};
    float g {};
    float b {};
    float a { 255 };

    operator sf::Color() const;
};

bool operator==(const fColor& left, const fColor& right);
bool operator!=(const fColor& left, const fColor& right);

fColor operator+(const fColor& left, const fColor& right);
fColor operator*(const fColor& left, float right);
fColor operator-(const fColor& left, const fColor& right);

#endif // FCOLOR_HPP

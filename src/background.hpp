#ifndef BACKGROUND_HPP
#define BACKGROUND_HPP

#include "object.hpp"

#include <SFML/Graphics.hpp>

class Background : public Object
{
public:
    Background() {}

    static const sf::Color bgColor;

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // BACKGROUND_HPP

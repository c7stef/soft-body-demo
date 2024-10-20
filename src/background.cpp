#include "background.hpp"

const sf::Color Background::bgColor { 250, 250, 250 };

void Background::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    target.clear(bgColor);
}

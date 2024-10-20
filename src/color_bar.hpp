#ifndef COLOR_BAR_HPP
#define COLOR_BAR_HPP

#include "arrow.hpp"

#include <SFML/Graphics.hpp>

#include <deque>

class ColorBar : public sf::Drawable
{
public:
    void reset();
    void close();

    void addColor(sf::Color color);
    void selectFirstColor();
    void selectNextColor();

    void applyColor();

    int colorCount() const { return static_cast<int>(colorRects.size()); }

    void update();

private:
    std::vector<sf::RectangleShape> colorRects {};
    std::deque<Arrow> selectArrows {};

    bool inView { false };

    static constexpr float width = 60.0f;
    static constexpr float smoothness = 0.1f;

    static constexpr float arrowMargin = 30.0f;
    static constexpr float arrowMoveOffset = 30.0f;

    static const sf::Color arrowColor;

    int selectedColorIndex {};

    void selectColor(int index);
    sf::Vector2f getArrowPositionAt(int index);

    Arrow& getArrow() { return selectArrows.front(); }

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // COLOR_BAR_HPP

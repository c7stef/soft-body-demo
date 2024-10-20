#include "color_bar.hpp"
#include "utilities.hpp"

const sf::Color ColorBar::arrowColor { 38, 52, 79, 120 };

void ColorBar::reset()
{
    colorRects.clear();
}

void ColorBar::close()
{
    inView = false;

    applyColor();
}

void ColorBar::addColor(sf::Color color)
{
    auto toFloat { [](auto value) { return static_cast<float>(value); } };

    if (colorCount() == 0)
    {
        inView = true;

        sf::RectangleShape rectangle { sf::Vector2f { width, toFloat(Util::windowSize.y) } };
        rectangle.setPosition({ toFloat(Util::windowSize.x), 0 });
        rectangle.setFillColor(color);

        colorRects.push_back(rectangle);
    } else
    {
        sf::RectangleShape rectangle { sf::Vector2f { width, 0 } };
        rectangle.setPosition({ toFloat(Util::windowSize.x) - width, toFloat(Util::windowSize.y) });
        rectangle.setFillColor(color);

        colorRects.push_back(rectangle);
    }

    if (selectArrows.size() != 0)
        selectColor(selectedColorIndex);
}

void ColorBar::selectFirstColor()
{
    selectArrows.push_front({});

    getArrow().setColorInstant({ sf::Color::Transparent });
    getArrow().setColor(arrowColor);

    getArrow().setPositionInstant(getArrowPositionAt(0) + sf::Vector2f { -arrowMoveOffset, 0 });

    selectColor(0);
}

void ColorBar::selectNextColor()
{
    selectColor(++selectedColorIndex);
}

void ColorBar::applyColor()
{
    getArrow().setPosition(getArrow().getPosition() + sf::Vector2f { arrowMoveOffset, 0 });
    getArrow().setColor(sf::Color::Transparent);
}

void ColorBar::selectColor(int index)
{
    selectedColorIndex = index;
    getArrow().setPosition(getArrowPositionAt(index));
}

void ColorBar::update()
{
    for (int i = 0; i < colorCount(); i++)
    {
        auto oldPosition { colorRects[i].getPosition() };

        sf::Vector2f targetPosition {
            static_cast<float>(Util::windowSize.x) + (inView ? -width : 2),
            static_cast<float>(Util::windowSize.y) / colorCount() * i
        };

        colorRects[i].setPosition(Util::lerp(oldPosition, targetPosition, smoothness));

        auto oldSize { colorRects[i].getSize() };

        sf::Vector2f targetSize {
            width,
            static_cast<float>(Util::windowSize.y) / colorCount()
        };

        colorRects[i].setSize(Util::lerp(oldSize, targetSize, smoothness));
    }

    for (auto& arrow : selectArrows)
        arrow.update();

    while (selectArrows.size() > 0 && selectArrows.back().isTransparent())
        selectArrows.pop_back();
}

sf::Vector2f ColorBar::getArrowPositionAt(int index)
{
    return {
        Util::windowSize.x - width - arrowMargin,
        static_cast<float>(Util::windowSize.y) / colorCount() * (index + 0.5f)
    };
}

void ColorBar::draw(sf::RenderTarget& target, [[maybe_unused]] sf::RenderStates states) const
{
    for (const auto& colorRect : colorRects)
        target.draw(colorRect);

    for (const auto& arrow : selectArrows)
        target.draw(arrow);
}

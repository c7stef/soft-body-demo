#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "fcolor.hpp"

#include <SFML/Graphics.hpp>

#include <functional>
#include <string>

class Button : public sf::Drawable
{
public:
    enum Style
    {
        Primary,
        Secondary
    };

    Button(std::string text, std::function<void()> action = {});

    void update();

    void setAction(std::function<void()> action) { callback = action; }
    void setPosition(sf::Vector2f position);
    void setStyle(Style newStyle);

    sf::Vector2f getBounds();

    void sendLeftButtonPressed(sf::Vector2f coords);
    void sendLeftButtonReleased(sf::Vector2f coords);
    void sendMouseMoved(sf::Vector2f coords);

    void enable() { enabled = true; updateTextColor(); }
    void disable() { enabled = false; updateTextColor(); }

private:
    sf::Text displayText {};
    std::function<void()> callback {};

    sf::Vector2f position {};
    Style style { Primary };

    bool mouseIn { false };
    bool mousePressed { false };

    static constexpr unsigned fontSize { 22 };
    static constexpr float outlineWidth { 3 };
    static constexpr float borderRadius { 8 };
    const fColor primaryColor { 107, 52, 235 };
    const fColor secondaryColor { 255, 255, 255 };
    const sf::Vector2f padding { 20, 10 };

    bool enabled { true };
    void updateTextColor();

    bool hitInside(sf::Vector2f coords) const;

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};

#endif // BUTTON_HPP

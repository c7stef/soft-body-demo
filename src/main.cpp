#include "background.hpp"
#include "button.hpp"
#include "fonts.hpp"
#include "graf.hpp"
#include "graph_colorizer.hpp"
#include "graph_force_system.hpp"
#include "utilities.hpp"

#include <SFML/Graphics.hpp>

#include <string>
#include <cmath>

sf::ContextSettings getContextSettings()
{
    sf::ContextSettings settings {};
    settings.antialiasingLevel = 16;
    return settings;
}

template<typename T>
sf::Vector2f getEventXY(const T& eventData)
{
    return
    {
        static_cast<float>(eventData.x),
        static_cast<float>(eventData.y)
    };
}

int main()
{
    Fonts::initFonts();

    sf::RenderWindow window {
        sf::VideoMode(Util::windowSize.x, Util::windowSize.y),
        "Greedy Coloring",
        sf::Style::Titlebar | sf::Style::Close,
        getContextSettings()
    };

    window.setFramerateLimit(60);

    Background bg {};

    Graf graf {};
    GraphColorizer colorizer { graf };
    GraphForceSystem forceSystem { graf };

    Button startColoringButton { "Start Coloring"};
    startColoringButton.setPosition({10, 10});

    Button loadGraphButton { "Load Graph", [&](){
        graf.openFileDialogAndLoad();
        forceSystem.reload();
    } };
    loadGraphButton.setPosition({startColoringButton.getBounds().x + 20, 10});
    loadGraphButton.setStyle(Button::Secondary);

    startColoringButton.setAction([&]()
        {
            loadGraphButton.disable();
            colorizer.start();
        }
    );

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    auto mouseCoords = getEventXY(event.mouseButton);

                    loadGraphButton.sendLeftButtonPressed(mouseCoords);
                    startColoringButton.sendLeftButtonPressed(mouseCoords);
                    forceSystem.sendLeftButtonPressed(mouseCoords);
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                auto mouseCoords = getEventXY(event.mouseMove);

                loadGraphButton.sendMouseMoved(mouseCoords);
                startColoringButton.sendMouseMoved(mouseCoords);
                forceSystem.sendMouseMoved(mouseCoords);
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    auto mouseCoords = getEventXY(event.mouseButton);

                    loadGraphButton.sendLeftButtonReleased(mouseCoords);
                    startColoringButton.sendLeftButtonReleased(mouseCoords);
                    forceSystem.sendLeftButtonReleased(mouseCoords);
                }
            }
        }

        graf.update();
        forceSystem.update();
        colorizer.update();

        if (colorizer.isStopped())
            loadGraphButton.enable();

        window.draw(bg);
        window.draw(graf);
        window.draw(colorizer);

        window.draw(startColoringButton);
        window.draw(loadGraphButton);

        window.display();
    }

    return 0;
}

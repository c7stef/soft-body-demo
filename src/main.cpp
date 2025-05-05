#include "background.hpp"
#include "button.hpp"
#include "fonts.hpp"
#include "mesh.hpp"
#include "mesh_force_system.hpp"
#include "utilities.hpp"
#include "scene.hpp"
#include "object.hpp"

#include <SFML/Graphics.hpp>

#include <ostream>
#include <string>
#include <cmath>
#include <memory>

#include <iostream>
#include <fstream>

static constexpr float meshGranularity = 40.f;

class MomentumObserver : public Object
{
public:
    MomentumObserver(std::weak_ptr<MeshForceSystem> forceSystem) :
        forceSystem{forceSystem}
    {
    }

    void update(float deltaTime) override
    {
        auto system = forceSystem.lock();
        if (system)
        {
            logFile << system->getMomentum() << std::endl;
            logFile2 << system->getAngularMomentum() << std::endl;
        }
    }

private:
    std::weak_ptr<MeshForceSystem> forceSystem;
    std::ofstream logFile{"momentum_log.txt"};
    std::ofstream logFile2{"angular_momentum_log.txt"};
};

std::unique_ptr<Scene> makeSimulationScene(const sf::Font& textFont)
{
    auto scene = std::make_unique<Scene>();

    scene->addObject(std::make_shared<Background>());

    auto mesh = std::make_shared<Mesh>(textFont);
    scene->addObject(mesh);

    auto forceSystem = std::make_shared<MeshForceSystem>(mesh);
    scene->addObject(forceSystem);

    std::weak_ptr<Mesh> weakMesh = mesh;
    std::weak_ptr<MeshForceSystem> weakForceSystem = forceSystem;

    auto loadMeshButton = std::make_shared<Button>(
        "Load Mesh",
        textFont,
        [weakMesh, weakForceSystem](){
            weakMesh.lock()->openFileDialogAndLoad(meshGranularity);
            weakForceSystem.lock()->reload();
        }
    );
    loadMeshButton->setPosition({10, 10});
    loadMeshButton->setStyle(Button::Secondary);

    scene->addObject(loadMeshButton);

    scene->addObject(std::make_shared<MomentumObserver>(forceSystem));

    return scene;
}

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
    auto textFont{Fonts::textFont()};

    sf::RenderWindow window {
        sf::VideoMode(Util::windowSize.x, Util::windowSize.y),
        "Softbody Demo",
        sf::Style::Titlebar | sf::Style::Close,
        getContextSettings()
    };

    window.setFramerateLimit(60);

    auto scene{makeSimulationScene(textFont)};

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
                    scene->sendLeftButtonPressed(mouseCoords);
                } else if (event.mouseButton.button == sf::Mouse::Right)
                {
                    auto mouseCoords = getEventXY(event.mouseButton);
                    scene->sendRightButtonPressed(mouseCoords);
                }
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                auto mouseCoords = getEventXY(event.mouseMove);
                scene->sendMouseMoved(mouseCoords);
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    auto mouseCoords = getEventXY(event.mouseButton);
                    scene->sendLeftButtonReleased(mouseCoords);
                }
            }
            else if (event.type == sf::Event::KeyPressed)
            {
                scene->sendKeyPressed(event.key.code);
            }
        }
        
        scene->update(0.016f);

        window.draw(*scene);
        window.display();
    }
    
    return 0;
}

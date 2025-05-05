#ifndef SCENE_HPP
#define SCENE_HPP

#include "object.hpp"

#include <SFML/Graphics.hpp>

#include <memory>

class Scene : public Object
{
public:
    Scene() = default;
    
    void update(float deltaTime) override
    {
        for (auto& object : objects)
            object->update(deltaTime);
    }
    
    void addObject(std::shared_ptr<Object> object)
    {
        objects.push_back(object);
    }

    void removeObject(std::shared_ptr<Object> object)
    {
        objects.erase(std::remove(objects.begin(), objects.end(), object), objects.end());
    }

    void draw(sf::RenderTarget& target, sf::RenderStates states) const override
    {
        for (const auto& object : objects)
            object->draw(target, states);
    }

    void sendLeftButtonPressed(sf::Vector2f coords) override
    {
        for (const auto& object : objects)
            object->sendLeftButtonPressed(coords);
    }

    void sendRightButtonPressed(sf::Vector2f coords) override
    {
        for (const auto& object : objects)
            object->sendRightButtonPressed(coords);
    }
    
    void sendLeftButtonReleased(sf::Vector2f coords) override
    {
        for (const auto& object : objects)
            object->sendLeftButtonReleased(coords);
    }
    
    void sendMouseMoved(sf::Vector2f coords) override
    {
        for (const auto& object : objects)
            object->sendMouseMoved(coords);
    }

    void sendKeyPressed(sf::Keyboard::Key key) override
    {
        for (const auto& object : objects)
            object->sendKeyPressed(key);
    }

private:
    std::vector<std::shared_ptr<Object>> objects{};
};

#endif // SCENE_HPP
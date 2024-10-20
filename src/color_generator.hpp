#ifndef COLOR_GENERATOR_HPP
#define COLOR_GENERATOR_HPP

#include <SFML/Graphics.hpp>

#include <string>
#include <vector>

#include "json.hpp"

class ColorGenerator
{
public:
    ColorGenerator() {}

    sf::Color getNextColor();
    void reset();

private:
    std::vector<sf::Color> buffer {};

    int currentBufferIndex {};

    void fillBuffer();
    void refillBuffer();

    std::string getFillPostData();
    std::string getRefillPostData();

    static const std::string apiUrl;
    static constexpr int persistentColors { 3 };

    bool usingFallback { false };
    int fallbackColorIndex { 0 };

    static const std::vector<sf::Color> fallbackColors;

    int bufferCount() { return static_cast<int>(buffer.size()); }

    nlohmann::json colorToJson(sf::Color color);

    std::string getRawApiData(const std::string& postData);
    nlohmann::json getJsonApiData(const std::string& postData);

    void fillBufferWithJson(const nlohmann::json& jsonData);

    static std::size_t requestCallback(void* data, std::size_t size, std::size_t nmemb, void* requestBuffer);
};

#endif // COLOR_GENERATOR_HPP

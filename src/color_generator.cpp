#include "color_generator.hpp"

#include <curl/curl.h>
#include "json.hpp"

#include <iostream>
#include <stdexcept>

const std::string ColorGenerator::apiUrl { "http://colormind.io/api/" };

const std::vector<sf::Color> ColorGenerator::fallbackColors {
    { 255, 145, 28 },
    { 244, 255, 28 },
    { 28, 255, 153 },
    { 47, 204, 224 },
    { 39, 109, 207 },
    { 102, 67, 240 },
    { 211, 98, 252 },
    { 252, 98, 206 }
};

sf::Color ColorGenerator::getNextColor()
{
    if (bufferCount() == 0)
        fillBuffer();

    if (currentBufferIndex == bufferCount())
        refillBuffer();

    return buffer[currentBufferIndex++];
}

void ColorGenerator::reset()
{
    buffer.clear();
    usingFallback = false;
}

void ColorGenerator::fillBuffer()
{
    auto jsonColors { getJsonApiData(getFillPostData()) };
    fillBufferWithJson(jsonColors);

    currentBufferIndex = 0;
}

void ColorGenerator::refillBuffer()
{
    auto jsonColors { getJsonApiData(getRefillPostData()) };

    buffer.clear();
    fillBufferWithJson(jsonColors);

    currentBufferIndex = persistentColors;
}

void ColorGenerator::fillBufferWithJson(const nlohmann::json& jsonData)
{
    for (auto& colorVecContainer : jsonData) {
        for (auto& colorVec : colorVecContainer)
            buffer.push_back({colorVec[0], colorVec[1], colorVec[2]});
    }
}

nlohmann::json ColorGenerator::colorToJson(sf::Color color)
{
    return nlohmann::json::array({color.r, color.g, color.b});
}

nlohmann::json ColorGenerator::getJsonApiData(const std::string& postData)
{
    if (!usingFallback)
    {
        try
        {
            std::string rawApiData { getRawApiData(postData) };

            nlohmann::json colorsJson { nlohmann::json::parse(rawApiData) };
            auto resultColors { colorsJson[0]["result"] };

            return resultColors[0];
        } catch (const std::system_error& sysError)
        {
            usingFallback = true;
            fallbackColorIndex = 0;
        }
    }

    // Using fallback
    nlohmann::json colorsJson {};
    for (int i = 0; i < 5; i++)
        colorsJson.push_back(colorToJson(fallbackColors[fallbackColorIndex + i]));
    fallbackColorIndex += 5 - persistentColors;

    return colorsJson;
}

std::string ColorGenerator::getFillPostData()
{
    nlohmann::json jsonPost {};
    jsonPost["model"] = "default";

    return jsonPost.dump();
}

std::string ColorGenerator::getRefillPostData()
{
    nlohmann::json jsonPost {};
    jsonPost["model"] = "default";

    nlohmann::json inputField {};

    for (int i = persistentColors; i >= 1; i--)
        inputField.push_back(colorToJson(buffer[bufferCount() - i]));

    for (int i = 0; i < bufferCount() - persistentColors; i++)
        inputField.push_back("N");

    jsonPost["input"] = inputField;

    return jsonPost.dump();
}

std::string ColorGenerator::getRawApiData(const std::string& postData)
{
    std::string requestDataRaw {};

    CURL* curlHandle { curl_easy_init() };

    if (curlHandle)
    {
        curl_easy_setopt(curlHandle, CURLOPT_URL, apiUrl.c_str());
        curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, postData.c_str());

        curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, requestCallback);
        curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &requestDataRaw);

        CURLcode result = curl_easy_perform(curlHandle);

        if (result == 0)
            curl_easy_cleanup(curlHandle);
        else
            throw std::system_error {};
    } else
        throw std::system_error {};

    return requestDataRaw;
}

std::size_t ColorGenerator::requestCallback(void* data, std::size_t size, std::size_t nmemb, void* requestBuffer)
{
    std::string& bufferString { *(std::string*)requestBuffer };

    auto payloadSize { size * nmemb };
    std::string newData((char*)data, payloadSize);

    bufferString += newData;

    return payloadSize;
}

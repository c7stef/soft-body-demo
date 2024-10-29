#include "fonts.hpp"

#include <filesystem>

std::filesystem::path font_dir_path{FONT_DIR};

sf::Font Fonts::textFont()
{
    sf::Font font{};
    font.loadFromFile(font_dir_path / "Poppins-Regular.ttf");
    return font;
};

sf::Font Fonts::numberFont()
{
    sf::Font font{};
    font.loadFromFile(font_dir_path / "Lato-Regular.ttf");
    return font;
};

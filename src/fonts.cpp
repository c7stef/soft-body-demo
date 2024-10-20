#include "fonts.hpp"

#include <filesystem>

sf::Font Fonts::textFont {};
sf::Font Fonts::numberFont {};

void Fonts::initFonts()
{
    std::filesystem::path font_dir_path{FONT_DIR};

    textFont.loadFromFile(font_dir_path / "Poppins-Regular.ttf");
    numberFont.loadFromFile(font_dir_path / "Lato-Regular.ttf");
}

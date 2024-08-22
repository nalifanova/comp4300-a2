#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <iostream>
#include <SFML/Graphics.hpp>

std::string getFullPath(const std::string& filename, bool is_font = false);

void loadFont(sf::Font& font, std::string& path);

#endif //FILE_UTILS_H

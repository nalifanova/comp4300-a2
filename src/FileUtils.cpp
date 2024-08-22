#include "FileUtils.h"

#include <fstream>

/**
 * Get path to defined file in assets
 * @param filename filename or filepath
 * @param is_font includes/excludes 'data' folder
 * @return string path
 */
std::string getFullPath(const std::string& filename, bool is_font)
{
    const std::filesystem::path cwd = std::filesystem::current_path();
    const std::filesystem::path paragraph_dir = cwd.parent_path().parent_path()
                                                / "assets";
    std::string folder = "/data/";
    if (is_font)
        folder = "/";
    std::string filename_ = paragraph_dir.string() + folder + filename;
    return filename_;
}

void loadFont(sf::Font& font, std::string& path)
{
    auto filepath = getFullPath(path, true);
    std::cout << "Font filepath: " << filepath << "\n";
    if (!font.loadFromFile(filepath))
    {
        std::cerr << "Could not load font!\n";
        exit(-1);
    }
}
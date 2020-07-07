#include <iostream>
#include <string>
#include "assetcontainer.h"

AssetContainer editor_assets;

AssetContainer::AssetContainer()
{
    init_images();
}

void AssetContainer::init_images()
{
    std::cout << "Loading program images..." << std::endl;
    QString filename;
    std::string tileset_path = "assets/tiles.png";
    std::string background_path = "assets/tiles.png";

    try {
        filename = QString(tileset_path.c_str());
        images["tiles"] = QImage(filename);
        if (images["tiles"].isNull()) {
            throw std::runtime_error("Couldn't load tileset image.");
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }

    try {
        filename = QString(background_path.c_str());
        images["background"] = QImage(filename);
        if (images["background"].isNull()) {
            throw std::runtime_error("Couldn't load background image.");
        }
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

QImage AssetContainer::get_image(std::string name)
{
    return images[name];
}


#include <iostream>
#include <string>
#include <utility>
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
    std::string background_path = "assets/background.png";
    std::string objects_path = "assets/objects.png";

    std::vector<std::pair<std::string, std::string>> assets_to_load;
    assets_to_load.push_back(std::make_pair("tiles", tileset_path));
    assets_to_load.push_back(std::make_pair("background", background_path));
    assets_to_load.push_back(std::make_pair("object", objects_path));

    for (auto asset : assets_to_load) {
        try {
            filename = QString(asset.second.c_str());
            images[asset.first] = QImage(filename);
            if (images[asset.first].isNull()) {
                throw std::runtime_error("Couldn't load image: " + asset.second);
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }
}

QImage AssetContainer::get_image(std::string name)
{
    return images[name];
}


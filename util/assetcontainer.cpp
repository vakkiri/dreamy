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
    assets_to_load.push_back(std::make_pair("objects", objects_path));

    for (auto asset : assets_to_load) {
        try {
            filename = QString(asset.second.c_str());
            images[asset.first] = QImage(filename);
            if (images[asset.first].isNull()) {
                throw std::runtime_error("Couldn't load image: " + asset.second);
            } else {
                assets[asset.first] = std::vector<Asset>();
            }
        } catch (const std::exception& e) {
            std::cout << e.what() << std::endl;
        }
    }

    // wow, this would actually be a good place to init all those tiles...

    assets["objects"].push_back(Asset{0.f , 0.f, 32.f, 32.f});
    assets["objects"].push_back(Asset{32.f, 0.f, 32.f, 32.f});

}

QImage AssetContainer::get_image(std::string name)
{
    return images[name];
}

std::vector<Asset>& AssetContainer::get_assets(std::string name) {
    return assets[name];
}

std::unordered_map<std::string, std::vector<Asset>>& AssetContainer::get_assets() {
    return assets;
}

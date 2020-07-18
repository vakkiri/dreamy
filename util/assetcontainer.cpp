#include <iostream>
#include <fstream>
#include <string>
#include <utility>
#include <sstream>
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
    std::string asset_conf_path = "config/assets.conf";
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

    // object types: 0 = passable tile, 1 = solid tile, 2.. = custom
    for (int i = 0; i < images["tiles"].width(); i += 16.f) {
        assets["tiles"].push_back(Asset{"tiles", float(i), 0.f, 16.f, 16.f, 0});
    }

    try {
        std::ifstream file(asset_conf_path);
        if (file.bad()) {
            throw std::runtime_error("Couldn't load asset conf file.");
        } else {
            std::string line;
            std::string group;
            int object_id = 2;  // first available id, since tiles take 0 and 1
            float s, t, w, h;
            while (std::getline(file, line) && line.length() > 0) {
                std::vector<std::string> tokens;
                std::istringstream linestream(line);
                std::string token;
                char delim = '\t';
                while (std::getline(linestream, token, delim)) {
                    tokens.push_back(token);
                }
                group = tokens[0];
                s = std::stof(tokens[1]);
                t = std::stof(tokens[2]);
                w = std::stof(tokens[3]);
                h = std::stof(tokens[4]);
                s = s / get_image(group).width();
                t = t / get_image(group).height();
                assets["objects"].push_back(Asset{group, s, t, w, h, object_id++});
            }
        }

    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
    }
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

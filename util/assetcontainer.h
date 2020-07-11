#ifndef ASSETCONTAINER_H
#define ASSETCONTAINER_H

#include <unordered_map>
#include <string>
#include <QImage>
#include "../types/types.h"

class AssetContainer
{
public:
    AssetContainer();
    QImage get_image(std::string name);
    std::vector<Asset>& get_assets(std::string name);
    std::unordered_map<std::string, std::vector<Asset>>& get_assets();
private:
    void init_images();
    std::unordered_map<std::string, QImage> images;
    std::unordered_map<std::string, std::vector<Asset>> assets;
};

extern AssetContainer editor_assets;

#endif // ASSETCONTAINER_H

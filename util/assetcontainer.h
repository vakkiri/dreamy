#ifndef ASSETCONTAINER_H
#define ASSETCONTAINER_H

#include <unordered_map>
#include <string>
#include <QImage>

class AssetContainer
{
public:
    AssetContainer();
    QImage get_image(std::string name);

private:
    void init_images();
    std::unordered_map<std::string, QImage> images;
};

extern AssetContainer editor_assets;

#endif // ASSETCONTAINER_H

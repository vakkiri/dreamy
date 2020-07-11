#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>

struct TileInfo {
    float s;
    float t;
    float x;
    float y;
    bool solid;
    int layer;
};

struct TextureData {
    float s;
    float t;
};

struct Asset {
    float s;
    float t;
    float w;
    float h;
};

struct AssetInstance {
    std::string image_name;
    float x;
    float y;
    float s;
    float t;
    float w;
    float h;
};

#endif // TYPES_H

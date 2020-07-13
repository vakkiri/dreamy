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
    std::string group;
    float s;
    float t;
    float w;
    float h;
    int type;
};

struct AssetInstance {
    std::string group;
    float x;
    float y;
    float s;
    float t;
    float w;
    float h;
    int type;
};

struct AssetOutput {
    int16_t type;
    int16_t x;
    int16_t y;
};

#endif // TYPES_H

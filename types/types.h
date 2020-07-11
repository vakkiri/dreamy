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
    std::string image_name;
    float s;
    float t;
    float w;
    float h;
};

#endif // TYPES_H

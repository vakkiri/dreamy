#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>

enum EditMode {
    ADD_MODE,
    PORTAL_MODE,
    WATER_MODE,
    NUM_MODES
};

struct Portal {
    float x;
    float y;
    float w;
    float h;
    float destx;
    float desty;
    int32_t destlevel;
};

struct Water {
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
};

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

struct TileSavedAsset {
    int16_t type;
    int16_t x;
    int16_t y;
    int16_t index;
};

struct ObjectSavedAsset {
    int16_t type;
    int16_t x;
    int16_t y;
};


#endif // TYPES_H

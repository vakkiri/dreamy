#include "utils.h"
#include "widgets/const.h"

bool clickInTile(float clickx, float clicky, float tilex, float tiley) {
    bool xin = clickx >= tilex && clickx <= tilex + TILE_WIDTH;
    bool yin = clicky >= tiley && clicky <= tiley + TILE_WIDTH;

    return xin && yin;
}

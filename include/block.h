#ifndef BLOCK_H
#define BLOCK_H

#include "vector.h"

typedef struct {
    Vector2 pos;
    SDL_Texture *texture;
} Block;

#endif

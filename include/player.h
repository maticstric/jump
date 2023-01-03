#ifndef PLAYER_H
#define PLAYER_H

#include "vector.h"

typedef struct {
    Vector2 pos;
    Vector2 vel;
    SDL_Texture *texture;
    bool is_grounded;
} Player;

extern void player_update();

#endif

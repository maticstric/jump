#ifndef APP_H
#define APP_H

#include "player.h"
#include "block.h"

#define PLAYER_SIZE 32
#define BLOCK_SIZE 32
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define NUM_BLOCKS 20

typedef struct {
    SDL_Renderer *renderer;
    SDL_Window *window;
    bool is_running;
    Player player;
    Block blocks[NUM_BLOCKS];
    bool jump;
    bool left;
    bool right;
    bool up;
    bool down;
} App;

#endif

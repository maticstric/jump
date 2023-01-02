#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "vector.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int NUM_BLOCKS = 20;
const int BLOCK_SIZE = 32;
const int PLAYER_SIZE = 32;
const char *TITLE = "jump";

typedef struct {
    Vector2 pos;
    Vector2 vel;
    SDL_Texture *texture;
    bool is_grounded;
} Player;

typedef struct {
    Vector2 pos;
    SDL_Texture *texture;
} Block;

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

App app = {0};

void init_SDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    app.window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (!app.window) {
        fprintf(stderr, "SDL_CreateWindow Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    app.renderer = SDL_CreateRenderer(app.window, -1, SDL_RENDERER_ACCELERATED);

    if (!app.renderer) {
        fprintf(stderr, "SDL_CreateRenderer Error: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

SDL_Texture* load_texture(char *file_path) {
    SDL_Texture *texture = IMG_LoadTexture(app.renderer, file_path);

    if(!texture) {
        fprintf(stderr, "IMG_LoadTexture Error: %s\n", SDL_GetError());
    }

    return texture;
}

void init_player() {
    app.player = (Player) { .pos = (Vector2){ .x = 240, .y = 200 },
                            .is_grounded = false };

    app.player.texture = load_texture("res/images/player.png");
}

void init_blocks() {
    for (int i = 2; i < NUM_BLOCKS - 2; ++i) {
        app.blocks[i] = (Block){ .pos = (Vector2){ .x = i * 32, .y = 384 + 2*i } };
        app.blocks[i].texture = load_texture("res/images/block.png");
    }
    
    app.blocks[NUM_BLOCKS - 2] = (Block){ .pos = (Vector2){ .x = 240, .y = 352 } };
    app.blocks[NUM_BLOCKS - 2].texture = load_texture("res/images/block.png");
    app.blocks[NUM_BLOCKS - 1] = (Block){ .pos = (Vector2){ .x = 240, .y = 320 } };
    app.blocks[NUM_BLOCKS - 1].texture = load_texture("res/images/block.png");
}

void init() {
    app.is_running = true;

    init_SDL();
    init_player();
    init_blocks();
}

void handle_keydown(SDL_KeyboardEvent event) {
    int scancode = event.keysym.scancode;

    if (event.repeat != 0) return;

    if (scancode == SDL_SCANCODE_SPACE) app.jump = true;
    if (scancode == SDL_SCANCODE_LEFT) app.left = true;
    if (scancode == SDL_SCANCODE_RIGHT) app.right = true;
    if (scancode == SDL_SCANCODE_UP) app.up = true;
    if (scancode == SDL_SCANCODE_DOWN) app.down = true;
}

void handle_keyup(SDL_KeyboardEvent event) {
    int scancode = event.keysym.scancode;

    if (event.repeat != 0) return;

    if (scancode == SDL_SCANCODE_SPACE) app.jump = false;
    if (scancode == SDL_SCANCODE_LEFT) app.left = false;
    if (scancode == SDL_SCANCODE_RIGHT) app.right = false;
    if (scancode == SDL_SCANCODE_UP) app.up = false;
    if (scancode == SDL_SCANCODE_DOWN) app.down = false;
}

void draw_player() {
    SDL_Rect dest = { (int)app.player.pos.x, (int)app.player.pos.y };
    SDL_QueryTexture(app.player.texture, NULL, NULL, &dest.w, &dest.h);

    SDL_RenderCopy(app.renderer, app.player.texture, NULL, &dest);
}

void draw_blocks() {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        Block block = app.blocks[i];

        SDL_Rect dest = { (int)block.pos.x, (int)block.pos.y };
        SDL_QueryTexture(app.blocks[i].texture, NULL, NULL, &dest.w, &dest.h);

        SDL_RenderCopy(app.renderer, app.blocks[i].texture, NULL, &dest);
    }
}

void draw() {
    SDL_RenderClear(app.renderer);

    draw_player();
    draw_blocks();

    SDL_RenderPresent(app.renderer);
}

/*
 * The functions 'player_add_vertical_pos' and 'player_add_horizontal_pos'
 * check if this update to the position will result in a collision. If it will,
 * calculate the minimum you have to change it by to not cause a collision and
 * then add that to the new position. It it will not, just add pos_addition
 * to the new position.
 */
Vector2 player_add_vertical_pos(Vector2 current_pos, float pos_addition) {
    if (pos_addition == 0) { return current_pos; }

    Vector2 new_pos = { .x = current_pos.x, 
                        .y = current_pos.y + pos_addition };

    // It might intersect multiple blocks. In this case, pick the max
    int max_intersection = 0; 
    
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        Block block = app.blocks[i];

        SDL_Rect block_rect = { .x = block.pos.x,
                                .y = block.pos.y,
                                .w = BLOCK_SIZE,
                                .h = BLOCK_SIZE };

        SDL_Rect player_rect = { .x = new_pos.x,
                                 .y = new_pos.y,
                                 .w = PLAYER_SIZE,
                                 .h = PLAYER_SIZE };

        SDL_Rect intersection;

        if (SDL_IntersectRect(&block_rect, &player_rect, &intersection)) {
            if (intersection.h > max_intersection) {
                max_intersection = intersection.h;
            }
        }
    }
    
    if (max_intersection != 0) {
        // Position adjustment should be made opposite the direction of movement
        if (pos_addition < 0) { 
            pos_addition += max_intersection;
        } else { // pos_addition > 0
            pos_addition -= max_intersection;
        }
    }

    return (Vector2) { .x = current_pos.x, .y = current_pos.y + pos_addition };
}

Vector2 player_add_horizontal_pos(Vector2 current_pos, float pos_addition) {
    if (pos_addition == 0) { return current_pos; }

    Vector2 new_pos = { .x = current_pos.x + pos_addition,
                        .y = current_pos.y };

    // It might intersect multiple blocks. In this case, pick the max
    int max_intersection = 0; 
    
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        Block block = app.blocks[i];

        SDL_Rect block_rect = { .x = block.pos.x,
                                .y = block.pos.y,
                                .w = BLOCK_SIZE,
                                .h = BLOCK_SIZE };

        SDL_Rect player_rect = { .x = new_pos.x,
                                 .y = new_pos.y,
                                 .w = PLAYER_SIZE,
                                 .h = PLAYER_SIZE };

        SDL_Rect intersection;

        if (SDL_IntersectRect(&block_rect, &player_rect, &intersection)) {
            if (intersection.w > max_intersection) {
                max_intersection = intersection.w;
            }
        }
    }
    
    if (max_intersection != 0) {
        // Position adjustment should be made opposite the direction of movement
        if (pos_addition < 0) {
            pos_addition += max_intersection;
        } else { // pos_addition > 0
            pos_addition -= max_intersection;
        }
    }

    return (Vector2) { .x = current_pos.x + pos_addition, .y = current_pos.y };
}

void update_is_grounded() {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        Block block = app.blocks[i];

        SDL_Rect block_rect = { .x = block.pos.x,
                                .y = block.pos.y,
                                .w = BLOCK_SIZE,
                                .h = BLOCK_SIZE };

        SDL_Rect player_rect = { .x = app.player.pos.x,
                                 .y = app.player.pos.y,
                                 .w = PLAYER_SIZE,
                                 .h = PLAYER_SIZE };

        // If moving the player down by one causes a collision, then player is grounded
        player_rect.y += 1;

        if (SDL_HasIntersection(&block_rect, &player_rect)) {
            app.player.is_grounded = true;
            return;
        }
    }

    app.player.is_grounded = false;
}

void handle_player_movement() {
    if (app.left && !app.right) {
        app.player.vel.x = -2;
    } else if (app.right && !app.left) {
        app.player.vel.x = 2;
    } else {
        app.player.vel.x = 0;
    }

    Vector2 new_pos = player_add_horizontal_pos(app.player.pos, app.player.vel.x);
    new_pos = player_add_vertical_pos(new_pos, app.player.vel.y);

    app.player.pos = new_pos;

    // Gravity
    if (!app.player.is_grounded) {
        app.player.vel.y += 0.1;
    } else {
        app.player.vel.y = 0;
    }
}

void update() {
    handle_player_movement();
    update_is_grounded();
}

void poll_events() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            app.is_running = false;
        } else if (event.type == SDL_KEYDOWN) {
            handle_keydown(event.key);
        } else if (event.type == SDL_KEYUP) {
            handle_keyup(event.key);
        }
    }
}

void cleanup() {
    SDL_DestroyWindow(app.window);
    SDL_DestroyRenderer(app.renderer);
}

int main(int argc, char **argv) {
    init();

    while (app.is_running) {
        poll_events();
        update();
        draw();
    }

    cleanup();

    return 0;
}

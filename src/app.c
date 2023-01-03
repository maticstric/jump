#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>

#include "app.h"
#include "block.h"
#include "player.h"
#include "vector.h"

const char *TITLE = "jump";

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

void update() {
    player_update();
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

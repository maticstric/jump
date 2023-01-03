#include <SDL2/SDL.h>
#include <stdbool.h>

#include "app.h"
#include "vector.h"

extern App app;

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
        SDL_Rect block_rect = { .x = app.blocks[i].pos.x,
                                .y = app.blocks[i].pos.y,
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

void player_update_is_grounded() {
    for (int i = 0; i < NUM_BLOCKS; ++i) {
        SDL_Rect block_rect = { .x = app.blocks[i].pos.x,
                                .y = app.blocks[i].pos.y,
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

void player_handle_movement() {
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

void player_update() {
    player_handle_movement();
    player_update_is_grounded();
}

/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
* 
* @filename		level.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli, Law Zong Han, Dalton Koh
* @DP emails	muhammadnurfadzly.b@digipen.edu, zonghan.law@digipen.edu
                d.koh@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This interface involves enemy waves, player interactions,
                and level progression. Enums are included for different spawn
                waves and game levels, as well as function declarations for 
                managing game states (such as pausing, victory, and game over
                conditions).

                It also handles rendering game elements, such as health bars,
                enemies, and user interfaces, as well as updating the player's
                actions and spawning mechanics. Debugging functions are
                provided.
******************************************************************************/

#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include "cprocessing.h"

enum SpawnWave {
    WAVE_1 = 1,
    WAVE_2 = 2,
    WAVE_3 = 3
};

enum GameLevel {
    GAME_LEVEL_ONE = 1,
    GAME_LEVEL_TWO = 2
};

extern enum SpawnWave currentWave;
extern bool enableShooting;
extern bool isPaused, isWin;

CP_Image init_level(enum GameLevel level, float level_width, float level_height, float window_width, float window_height);

bool try_pause(float window_width, float window_height);

void next_wave_or_level(void);

void _render_victory_page(float window_width, float window_height);

bool try_game_over(float window_width, float window_height, CP_Image cursor);

void spawn_enemies(float dt);

void player_placeables(float mouse_x, float mouse_y, float dt);

void render_health_bars(float dt);

void render_health_heart(struct Rectangle entity, int health, int maxHealth);

void render_ui(float window_width, float window_height, float mouse_x, float mouse_y, float dt);

void render_new_wave_counter(float window_width, float window_height);

void render_entities(float dt, float mouse_x);

void enemy_move_attack(float dt);

const struct Rectangle* get_world_clips(int* num);

void check_collision(void);

void place_barrier(int index, float x, float y, int barrierLvl);

void place_ally(int index, float x, float y);

void buff_update(float dt);

void debug_render_enemy_grid(void);

void debug_render_boss_grid(void);

void debug_render_clips(void);

void debug_render_hitboxes(void);

void dispose_resources(void);
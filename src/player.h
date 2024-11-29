/******************************************************************************
* @filename		player.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file declares the functions necessary for player movement,
                instantiating its initial values, attributes such as health,
                speed, exp and so on. Also handles taking damage from sources,
                and rendering.
******************************************************************************/

#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"
#include "flicker.h"
#include "buff.h"
#include "projectile.h"

#define PLAYER_HITBOX_OFFSET_X(playerWidth) ( 27.f / 62.f * playerWidth)
#define PLAYER_HITBOX_OFFSET_Y(playerHeight) ( 16.f / 253.f * playerHeight)
#define CHOCOSHOOTER_FRAMES 6

// to bring over to next level (map)
extern struct ChocoShooter playerInstance;

struct ChocoShooter {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    CP_Image chocoFrames[CHOCOSHOOTER_FRAMES];

    enum Movement movement;
    int health, maxHealth;
    struct Rectangle hitbox;
    struct FlickerState flickerState;
    int attackPower;   
    float attackSpeed;
    int shield; 
    int activeBuffSlot, activeBuffType, activeBuffLevel;
    float exp;
    float expGained;
    float playerMaxExp;
    int playerLevel;
    enum ProjectileType currentProjectileType;
    bool isReflectingDmg;

    int currentFrame; // Index of the current frame being rendered
    float frameTime;  // Time per frame
    float timeAccumulator;
    int lastPausedFrame;
};

int player_take_damage(struct ChocoShooter* player, int damage);
struct ChocoShooter init_player(CP_Vector pos, float speed, int maxHealth, int atk, float fireRate, float frameTime);
void move_player(struct ChocoShooter* player, float dt, float level_width, float level_height, const struct Rectangle* clip_entities, int clip_count);
void render_player(struct ChocoShooter* player, int alpha, float deltaTime);
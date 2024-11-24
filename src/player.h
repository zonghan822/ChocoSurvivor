#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"
#include "flicker.h"
#include "buff.h"
#include "projectile.h"

#define PLAYER_HITBOX_OFFSET_X(playerWidth) (- 1 / 33.f * playerWidth)
#define PLAYER_HITBOX_OFFSET_Y(playerHeight) (- 8.f / 77.f * playerHeight)

// to bring over to next level (map)
extern struct ChocoShooter playerInstance;

struct ChocoShooter {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
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
    int playerMaxExp;
    int playerLevel;
    enum ProjectileType currentProjectileType;
    bool isReflectingDmg;
};

int player_take_damage(struct ChocoShooter* player, int damage);
struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth, int atk, float fireRate);
void move_player(struct ChocoShooter* player, float dt, float level_width, float level_height);
void render_player(struct ChocoShooter player, float alpha);
void render_health_bar(struct ChocoShooter* player, float dt, float windowWidth, float windowHeight, float meterX, float meterY, float meterWidth, float meterHeight);
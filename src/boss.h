/******************************************************************************
* @filename		boss.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This header file declares the `Boss` struct and functions
                for managing a boss character in the game, including movement,
                attacks, health, and states. It includes specific functions
                for the first boss (`boss_one`), such as primary and secondary
                attacks, movement, and damage handling. Unused functions
                for a scrapped second boss (boss_two) are also present
                but can be removed for clarity.
******************************************************************************/

#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"
#include "camera.h"
#include "astar.h"

#define BOSS_WIDTH 180.f
#define BOSS_HITBOX_OFFSET_X(bossWidth) (- 143.f/432.f * bossWidth)
#define BOSS_FRAMES 6

enum BossState {
    BOSS_STATE_UNSPAWNED = -1,
    BOSS_STATE_DIED = 0,
    BOSS_STATE_MOVING_TO_TOWER,
    BOSS_STATE_MOVING_TO_PLAYER,
    BOSS_STATE_MOVING_TO_BARRIER,
    BOSS_STATE_ATTACKING_PRIMARY,
    BOSS_STATE_ATTACKING_SECONDARY,
    BOSS_STATE_DETOUR,
    BOSS_STATE_IDLE
};

struct Boss {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    CP_Image bossFrames[BOSS_FRAMES];
    enum BossState state;
    int health, maxHealth;
    CP_Vector moveDirection;
    void (*move)(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount);
    void (*primaryAttack)(struct Boss*, struct ChocoShooter*, struct DefenseTarget*, float);
    int (*canPrimaryAttack)(struct Boss, struct ChocoShooter, struct DefenseTarget);
    void (*secondaryAttack)(struct Boss*, struct ChocoShooter*, struct DefenseTarget*, float);
    int (*canSecondaryAttack)(struct Boss, struct ChocoShooter);
    struct Rectangle hitbox;
    float attackSpeed;
    int attackPower;
    bool isTakingDamage;

    int currentFrame; // Index of the current frame being rendered
    float frameTime;  // Time per frame
    float timeAccumulator;
    int lastPausedFrame;
};

struct Boss init_boss_one(float width, float speed, int health, int attack_power, float attack_speed);
//struct Boss init_boss_two(float width, float speed, int health, int attack_power, float attack_speed);
void render_boss(struct Boss);
int boss_take_damage(struct Boss* boss, int damage);
void spawn_boss(struct Boss* boss, float x, float y);
void dispose_boss(struct Boss* boss);

void boss_one_pri_atk(struct Boss*, struct ChocoShooter*, struct DefenseTarget* tower, float dt);
void render_boss_one_pri_atk_telegraph(void);
int can_boss_one_pri_atk(struct Boss, struct ChocoShooter, struct DefenseTarget);
void boss_one_sec_atk(struct Boss*, struct ChocoShooter*, struct DefenseTarget* tower, float dt);
void render_boss_one_sec_atk_telegraph(struct Boss boss, struct ChocoShooter player);
int can_boss_one_sec_atk(struct Boss, struct ChocoShooter);
void boss_one_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount);

/*
void boss_two_pri_atk(struct Boss*, struct ChocoShooter*, struct DefenseTarget*, float dt);
int can_boss_two_pri_atk(struct Boss, struct ChocoShooter, struct DefenseTarget*);
void boss_two_sec_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_two_sec_atk(struct Boss, struct ChocoShooter);
void boss_two_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount);
*/

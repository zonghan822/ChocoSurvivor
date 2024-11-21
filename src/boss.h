#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"
#include "camera.h"
#include "astar.h"

#define BOSS_WIDTH 225.f

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
    enum BossState state;
    int health, maxHealth;
    CP_Vector moveDirection;
    void (*move)(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, int nodeCount);
    void (*primaryAttack)(struct Boss*, struct ChocoShooter*, float);
    int (*canPrimaryAttack)(struct Boss, struct ChocoShooter);
    void (*secondaryAttack)(struct Boss*, struct ChocoShooter*, float);
    int (*canSecondaryAttack)(struct Boss, struct ChocoShooter);
    struct Rectangle hitbox;
    float attackSpeed;
    int attackPower;
    bool isTakingDamage;
};

struct Boss init_boss_one(float width, float speed, int health, int attack_power, float attack_speed);
struct Boss init_boss_two(float width, float speed, int health, int attack_power, float attack_speed);
void render_boss(struct Boss, struct Camera cam);
int boss_take_damage(struct Boss* boss, int damage);
void spawn_boss(struct Boss* boss, float x, float y);
void dispose_boss(struct Boss* boss);

void boss_one_pri_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_one_pri_atk(struct Boss, struct ChocoShooter);
void boss_one_sec_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_one_sec_atk(struct Boss, struct ChocoShooter);
void boss_one_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, int nodeCount);

void boss_two_pri_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_two_pri_atk(struct Boss, struct ChocoShooter);
void boss_two_sec_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_two_sec_atk(struct Boss, struct ChocoShooter);
void boss_two_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, int nodeCount);

#pragma once

#include "cprocessing.h"
#include <stdbool.h>

#define MAX_PLAYER_PROJECTILES 20

enum ProjectileType {
    NORMAL_PROJECTILE = 0,
    FREEZING_PROJECTILE = 1 << 0,   // 0b_00000001 // 1
    TRACKING_PROJECTILE = 1 << 1,   // 0b_00000010 // 2
    FAST_PROJECTILE = 1 << 2,       // 0b_00000100 // 4
};

struct Projectile {
    CP_Vector pos;
    enum ProjectileType type;
    float speed;
    bool isActive;
    float range;
    float diameter;
    int dmg;
    CP_Vector shootDirection;
};

typedef struct {
    struct Projectile projectiles[20];
    int activeCount;
} ProjectilePool;

extern ProjectilePool playerProjectiles;
extern ProjectilePool allyProjectiles[];

struct Projectile init_projectile(enum ProjectileType projectileType, float projectileSpeed, float diameter, int dmg, CP_Vector direction);
void player_shoot(float dt, bool can_shoot, float mouse_x, float mouse_y);
void ally_shoot(float dt, struct ChocoRoach* enemies, int enemyCount);
void remove_projectile(ProjectilePool* pool, int index);
void update_projectiles(ProjectilePool* pool, float dt);
void update_all_projectiles(float dt);
void render_all_projectiles();
void render_projectiles(const ProjectilePool* pool);
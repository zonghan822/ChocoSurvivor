#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"

#define MAX_BULLET_COUNT 20

enum BulletType {
    NORMAL_BULLET,
    EXPOLSIVE_BULLET,
    ADHESIVE_BULLET,
    HEAL_BULLET
};

struct Bullet {
    CP_Vector pos;
    enum BulletType bulletType;
    float speed;
    BOOL isShooting;
    float shootRange;
    float diameter;
    int dmg;
    CP_Vector shootDirection;
};

struct ChocoShooter {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    enum Movement movement;
    int health, maxHealth;
    struct Rectangle hitbox;
    float fireRate;
    struct Bullet bullets[MAX_BULLET_COUNT];
    int atk;
};

int player_take_damage(struct ChocoShooter* player, int damage);
struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth, int atk, float fireRate);
void move_player(struct ChocoShooter* player, float dt);
void render_player(struct ChocoShooter player, float alpha, struct Camera cam);


struct Bullet init_bullet(enum BulletType bulletType, float bulletSpeed, float bulletDiameter, int dmg, CP_Vector bulletDirection);
void shoot_bullet(struct ChocoShooter *player, struct Bullet *bullet);
void update_pos_bullet(struct Bullet* bullet, float dt);
void render_bullet(struct Bullet bullet, struct Camera cam);
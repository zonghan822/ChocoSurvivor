#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"
#include "flicker.h"
#include "buffs.h"

#define MAX_BULLET_COUNT 20

enum BuffType {
    NO_BUFF,
    ATTACK_UP,
    ATTACK_SPEED_UP,
    HEALTH_UP,
};

struct Buffs {
    enum BuffType type[3];   // Current buff type (can be set to the latest buff type obtained)
    int buffExp;
    int maxBuffMeter;
    int attackP_lvl;      // Attack power level
    int attackS_lvl;      // Attack speed level
    int health_lvl;       // Health level
};

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
    struct Bullet bullets[MAX_BULLET_COUNT];
    struct FlickerState flickerState;
    struct Buffs buffs;
    int attackPower;   
    float attackSpeed;
    int shield;
};

int player_take_damage(struct ChocoShooter* player, int damage);
struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth, int attackPower, float attackSpeed);
void move_player(struct ChocoShooter* player, float dt);
void render_player(struct ChocoShooter player, float alpha, struct Camera cam);

struct Bullet init_bullet(enum BulletType bulletType, float bulletSpeed, float bulletDiameter, int dmg, CP_Vector bulletDirection);
void shoot_bullet(struct ChocoShooter *player, struct Bullet *bullet);
void update_pos_bullet(struct Bullet* bullet, float dt);
void render_bullet(struct Bullet bullet, struct Camera cam);

int update_buff_meter(struct ChocoShooter* player, int expGained);
//void render_buff_meter(struct Buffs buff);
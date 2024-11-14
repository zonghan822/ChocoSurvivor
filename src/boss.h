#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"
#include "camera.h"

struct Boss {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    int health, maxHealth;
    enum Movement movement;
    void (*move)(struct Boss*, CP_Vector move_to, float dt);
    void (*primaryAttack)(struct Boss*, struct ChocoShooter*, float);
    int (*canPrimaryAttack)(struct Boss, struct ChocoShooter);
    void (*secondaryAttack)(struct Boss*, struct ChocoShooter*, float);
    int (*canSecondaryAttack)(struct Boss, struct ChocoShooter);
    struct Rectangle hitbox;
    float attackSpeed;
    int attackPower;
};

struct Boss init_boss_one(float width, float speed, int health, int attack_power, float attack_speed);
void render_boss(struct Boss, struct Camera cam);
void boss_take_damge(struct Boss* boss, int damage);

void boss_one_pri_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_one_pri_atk(struct Boss, struct ChocoShooter);
void boss_one_sec_atk(struct Boss*, struct ChocoShooter*, float dt);
int can_boss_one_sec_atk(struct Boss, struct ChocoShooter);
void boss_one_move(struct Boss* boss, CP_Vector move_to, float dt);
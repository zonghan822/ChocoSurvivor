#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"
#include <stdbool.h>

struct Boss {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    int maxHealth;
    BOOL isFlipped; // Track if the boss should be facing left
    void (*attack)(struct Boss, struct ChocoShooter*);
    int (*canAttack)(struct Boss, struct ChocoShooter);
    struct Rectangle hitbox;
      int attackPower;
};

void melee_attack(struct Boss, struct ChocoShooter*);
int can_melee_attack(struct Boss boss, struct ChocoShooter player);

// Declare the Damage function (without implementation)
void Damage(struct Boss* boss, int damage);

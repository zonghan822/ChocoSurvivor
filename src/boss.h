#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"

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
};

void meleeAttack(struct Boss, struct ChocoShooter*);
int canMeleeAttack(struct Boss boss, struct ChocoShooter player);

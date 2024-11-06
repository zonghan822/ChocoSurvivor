#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"

struct ChocoShooter {
    CP_Vector pos;
    float width, height;
    float speed;
    CP_Image img;
    BOOL isFlipped;
    enum Movement movement;
    int health, maxHealth;
    struct Rectangle hitbox;
};

int player_take_damage(struct ChocoShooter* player, int damage);
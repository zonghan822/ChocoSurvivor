#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"

struct ChocoShooter {
    CP_Vector pos;
    float width;
    float height;
    float speed;
    CP_Image img;
    bool isFlipped;
    enum Movement movement;
    int health;
    int maxHealth;
};

int player_take_damage(struct ChocoShooter* player, int damage);
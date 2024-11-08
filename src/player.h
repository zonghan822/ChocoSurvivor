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

void takeDamage(struct ChocoShooter* player, int damage);
void move_player(struct ChocoShooter* player);
void update_pos_player(struct ChocoShooter* player, float dt);
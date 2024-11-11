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

struct ChocoShooter init_player(CP_Vector pos, float speed, float maxHealth);

void move_player(struct ChocoShooter* player, float dt, float level_width, float level_height);

void render_player(struct ChocoShooter player, float alpha, struct Camera cam);

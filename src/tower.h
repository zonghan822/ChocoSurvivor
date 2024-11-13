#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "camera.h"

struct DefenseTarget {
	CP_Image img;
	CP_Vector pos;
	float width;
	float height;
	int health;
	int maxHealth;
};

int tower_take_damage(struct DefenseTarget* tower, int damage);

int tower_revive_player(struct DefenseTarget* tower, struct ChocoShooter* player);

struct Rectangle get_tower_hitbox(struct DefenseTarget tower);

void render_tower(struct DefenseTarget tower, struct Camera cam);
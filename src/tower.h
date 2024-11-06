#pragma once
#include "cprocessing.h"
#include "utils.h"

struct DefenseTarget {
	CP_Vector pos;
	float width;
	float height;
	CP_Image img;
	int health;
	int maxHealth;
};

int tower_take_damage(struct DefenseTarget* tower, int damage);
int tower_revive_player(struct DefenseTarget* tower, struct ChocoShooter* player);
struct Rectangle get_tower_hitbox(struct DefenseTarget tower);
/******************************************************************************
* @filename		tower.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file declares the functions for rendering of tower,
				returning its hitbox and its logic for taking damage from
				various sources.
******************************************************************************/

#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "flicker.h"

struct DefenseTarget {
	CP_Image img;
	CP_Vector pos;
	float width;
	float height;
	int health;
	int maxHealth;
	struct FlickerState flickerState;
};

int tower_take_damage(struct DefenseTarget* tower, int damage);

int tower_revive_player(struct DefenseTarget* tower, struct ChocoShooter* player);

struct Rectangle get_tower_hitbox(struct DefenseTarget tower);

void render_tower(struct DefenseTarget tower);
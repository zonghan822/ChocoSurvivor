/******************************************************************************
* @filename		tower.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file handles rendering of tower, its hitbox and taking
				damage. It is a fairly simple file.
******************************************************************************/

#include "tower.h"
#include "player.h"
#include "cprocessing.h"
#include "utils.h"
#include "camera.h"
#include <stdio.h>

// Returns 1 if still alive, 0 if not
int tower_take_damage(struct DefenseTarget* tower, int damage) {
	if (tower->flickerState.isFlickering) {
		//printf("Tower is invincible and cannot take damage right now.\n");
		return 1;  // Skip the rest of the function if flickering
	}

	tower->health = CP_Math_ClampInt(tower->health - damage, 0, tower->maxHealth);

	if (tower->health > 0) {
		printf("Tower took %d damage, health is now %d\n", damage, tower->health);
		flicker(&(tower->flickerState), 1);
		return 1;
	}

	printf("Game Over!\n");
	return 0;
}

// Returns 1 if can revive, 0 if not
// 0 would mean game over
int tower_revive_player(struct DefenseTarget* tower, struct ChocoShooter* player) {
	if (tower_take_damage(tower, 1)) {
		printf("Player is revived.\n");
		player->health = player->maxHealth;
		return 1;
	}
	return 0; // game over
}

struct Rectangle get_tower_hitbox(struct DefenseTarget tower) {
	struct Rectangle rect = {
		.height = tower.height,
		.width = tower.width,
		.pos = tower.pos
	};

	return rect;
}

void render_tower(struct DefenseTarget tower) {
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Image_Draw(tower.img, tower.pos.x, tower.pos.y, tower.width, tower.height, 255);
}
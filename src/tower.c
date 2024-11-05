#include "tower.h"
#include "player.h"
#include "cprocessing.h"
#include "utils.h"
#include <stdio.h>

// Returns 1 if still alive, 0 if not
int tower_take_damage(struct DefenseTarget* tower, int damage) {
	tower->health = CP_Math_ClampInt(tower->health - damage, 0, tower->maxHealth);
	printf("Tower took %d damage, health is now %d\n", damage, tower->health);
	return tower->health > 0;
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
	struct Rectangle rect;
	rect.height = tower.height;
	rect.width = tower.width;
	rect.pos = tower.pos;

	return rect;
}
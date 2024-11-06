#include "boss.h"
#include "player.h"
#include <stdio.h>

void meleeAttack(struct Boss boss, struct ChocoShooter* player) {
	// Add delay

	printf("Melee attack\n");

	// Damage player
	player_take_damage(player, 1);
}

int canMeleeAttack(struct Boss boss, struct ChocoShooter player) {
	float minDistance = 125 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
	return CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance;
}
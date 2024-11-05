#include "boss.h"
#include "player.h"
#include <stdio.h>

void meleeAttack(struct Boss boss, struct ChocoShooter* player) {
	printf("Melee attack\n");

	// Knockback player

	// Damage player
	player_take_damage(player, 1);
}

int canMeleeAttack(struct Boss boss, struct ChocoShooter player) {
	return CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) < 125 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
}
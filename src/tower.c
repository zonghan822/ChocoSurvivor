#include "tower.h"
#include "player.h"
#include "cprocessing.h"
#include "utils.h"
#include "camera.h"
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
	struct Rectangle rect = {
		.height = tower.height,
		.width = tower.width,
		.pos = tower.pos
	};

	return rect;
}

void render_tower(struct DefenseTarget tower, struct Camera cam) {
	CP_Vector vec = get_cam_pos_rel(tower.pos, cam);

	// PLACEHOLDER
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Graphics_DrawRect(vec.x, vec.y, tower.width, tower.height);
	CP_Settings_TextSize(30);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Font_DrawText("TOWER", vec.x, vec.y);
}
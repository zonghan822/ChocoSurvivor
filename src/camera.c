#include "camera.h"
#include "player.h"
#include "cprocessing.h"

void update_camera(struct Camera* camera, struct ChocoShooter player, float level_width, float level_height) {

	// Update width and height in case of resolution change
	camera->width = CP_System_GetWindowWidth();
	camera->height = CP_System_GetWindowHeight();
	camera->pos.x = CP_Math_ClampFloat(player.pos.x, 0, level_width - camera->width);
	camera->pos.y = CP_Math_ClampFloat(player.pos.y, 0, level_height - camera->height);

	// Any more updates here
}

struct Camera init_camera(struct ChocoShooter player, float width, float height) {
	struct Camera cam = {
		.pos = player.pos,
		.width = width,
		.height = height
	};

	return cam;
}
#include "camera.h"
#include "player.h"
#include "cprocessing.h"

void update_camera(struct Camera* camera, struct ChocoShooter player, float level_width, float level_height) {

	// Update width and height in case of resolution change
	camera->width = (float)CP_System_GetWindowWidth();
	camera->height = (float)CP_System_GetWindowHeight();

	float halfWidth = camera->width / 2,
		halfHeight = camera->height / 2;

	camera->pos.x = CP_Math_ClampFloat(player.pos.x, halfWidth, level_width - halfWidth);
	camera->pos.y = CP_Math_ClampFloat(player.pos.y, halfHeight, level_height - halfHeight);

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

CP_Vector get_cam_pos_rel(CP_Vector vec, struct Camera cam) {
	float camHalfWidth = cam.width / 2,
		camHalfHeight = cam.height / 2;

	cam.pos.x -= camHalfWidth;
	cam.pos.y -= camHalfHeight;

	return CP_Vector_Subtract(vec, cam.pos);
}

CP_Vector get_world_pos_rel(CP_Vector vec, struct Camera cam) {
	float camHalfWidth = cam.width / 2,
		camHalfHeight = cam.height / 2;

	cam.pos.x -= camHalfWidth;
	cam.pos.y -= camHalfHeight;

	return CP_Vector_Add(vec, cam.pos);
}
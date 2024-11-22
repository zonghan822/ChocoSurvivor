#include "camera.h"
#include "player.h"
#include "cprocessing.h"

void update_camera(struct Camera* camera, struct ChocoShooter player, float level_width, float level_height) {

	// Update width and height in case of resolution change
	camera->width = (float)CP_System_GetWindowWidth();
	camera->height = (float)CP_System_GetWindowHeight();

	float halfWidth = camera->width / 2,
		halfHeight = camera->height / 2;

	// Clamp camera pos so it won't go beyond level height and width
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

// Get vector based on camera's pos
// Due to the level size bigger than window size, the camera
// position will represent the "view" of the level
// `vec` is the position of the entity
CP_Vector get_cam_pos_rel(CP_Vector vec, struct Camera cam) {
	// Get top left point of the camera
	// The camera height and width is basically the window size
	// Top left for correct rendering (0,0) is top left
	cam.pos.x -= cam.width / 2;
	cam.pos.y -= cam.height / 2;

	// Get the relative position of the entity/vector to
	// the camera position
	return CP_Vector_Subtract(vec, cam.pos);
}

CP_Vector get_world_pos_rel(CP_Vector vec, struct Camera cam) {
	float camHalfWidth = cam.width / 2,
		camHalfHeight = cam.height / 2;

	cam.pos.x -= camHalfWidth;
	cam.pos.y -= camHalfHeight;

	return CP_Vector_Add(vec, cam.pos);
}
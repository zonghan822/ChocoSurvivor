#pragma once
#include "cprocessing.h"

struct Camera {
	CP_Vector pos;
	float width, height;
};

void update_camera(struct Camera *camera, struct ChocoShooter player, float level_width, float level_height);

struct Camera init_camera(struct ChocoShooter player, float width, float height);

CP_Vector get_cam_pos_rel(CP_Vector vec, struct Camera cam);

CP_Vector get_world_pos_rel(CP_Vector vec, struct Camera cam);
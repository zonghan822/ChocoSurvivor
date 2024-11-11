#include <math.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"

void render_enemies(struct ChocoRoach roaches[], int count, struct Camera cam) {
	for (int i = 0; i < count; i++) {
		CP_Vector vec = get_cam_pos_rel(roaches[i].pos, cam);

		// Draw enemies
		float r = roaches[i].radius,
			d = roaches[i].degree;

		// Draw circle
		CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
		CP_Graphics_DrawCircle(vec.x, vec.y, r * 2.0f);

		// Draw triangle 
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Graphics_DrawTriangleAdvanced(vec.x + r, vec.y, vec.x - r * (float)cos(0.9),
			vec.y + r * (float)sin(0.9), vec.x - r * (float)cos(0.9), vec.y - r * (float)sin(0.9), d);
	}
}
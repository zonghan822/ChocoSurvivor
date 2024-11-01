#include <math.h>
#include "cprocessing.h"
#include "enemy.h"

void render_enemies(struct ChocoRoach roaches[], int count) {
	for (int i = 0; i < count; i++) {

		// Draw enemies
		float x = roaches[i].pos.x;
		float y = roaches[i].pos.y;
		float r = roaches[i].radius;
		float d = roaches[i].degree;

		// Draw circle
		CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
		CP_Graphics_DrawCircle(x, y, r * 2.0f);

		// Draw triangle 
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Graphics_DrawTriangleAdvanced(x + r, y, x - r * cos(0.9), y + r * sin(0.9), x - r * cos(0.9), y - r * sin(0.9), d);
	}

}
#include <math.h>
#include "cprocessing.h"
#include "enemy.h"

void render_enemies(struct ChocoRoach roaches[], int count) {
	for (int i = 0; i < count; i++) {

		CP_Image_Draw(
			roaches[i].img,
			roaches[i].pos.x,
			roaches[i].pos.y,
			roaches[i].width,
			roaches[i].height,
			255
		);
	}

}
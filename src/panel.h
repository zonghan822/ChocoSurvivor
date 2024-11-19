#pragma once

#include "hittest.h"
#include "cprocessing.h"

struct Panel {
	struct Rectangle rect;
	CP_Image tileMap;
};

struct Panel init_panel(float width, float height, float x, float y);
void render_panel(struct Panel panel);
void dispose_panel(struct Panel* panel);
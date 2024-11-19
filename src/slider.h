#pragma once
#include "hittest.h"
#include "cprocessing.h"

struct Slider {
	struct Rectangle track;
	struct Circle thumb;
	CP_Image tileMap;
};

struct Slider init_slider(float width, float height, float x, float y, float diameter);
void update_slider(struct Slider* slider, float value);
void render_slider(struct Slider slider);
void dispose_slider(struct Slider* slider);
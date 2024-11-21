#pragma once
#include "hittest.h"
#include "cprocessing.h"

enum SliderState {
	SLIDER_STATE_NONE,
	SLIDER_STATE_HELD
};

struct Slider {
	struct Rectangle track;
	struct Circle thumb;
	CP_Image tileMap;
	enum SliderState state;
	float value;
};

struct Slider init_slider(float width, float height, float x, float y, float diameter);
void update_slider_by_mouse(struct Slider* slider, float mouse_x, float mouse_y);
void update_slider(struct Slider* slider, float value);
void render_slider(struct Slider slider);
void dispose_slider(struct Slider* slider);
#pragma once

#include "hittest.h"

enum ButtonState {
	BUTTON_STATE_NONE,
	BUTTON_STATE_CLICKED,
	BUTTON_STATE_HOVERED
};

struct Button {
	struct Rectangle rect;
	enum ButtonState state;
	CP_Color rectColor;
	char* text;
	CP_Vector textPos;
	CP_Color textColor;
	float textSize;
};

struct Button init_button(float button_x, float button_y, float button_width, float button_height, CP_Color button_color, const char* text, float text_x, float text_y, CP_Color text_color, float text_size);
void render_button(struct Button button);
void update_button_state(struct Button* button, float mouse_x, float mouse_y);
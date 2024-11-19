#pragma once

#include "hittest.h"

enum ButtonState {
	BUTTON_STATE_NONE,
	BUTTON_STATE_CLICKED,
	BUTTON_STATE_HOVERED
};

enum ButtonType {
	BUTTON_TYPE_DEFAULT,
	BUTTON_TYPE_SUCCESS,
	BUTTON_TYPE_WARNING
};

struct Button {
	struct Rectangle rect;
	enum ButtonState state;
	enum ButtonType type;
	CP_Color rectColor;
	CP_Image tileMap;
	char* text;
	CP_Vector textPos;
	CP_Color textColor;
	float textSize;
};

struct Button init_button(float button_x, float button_y, float button_width, float button_height, enum ButtonType type, CP_Color button_color, const char* text, CP_Color text_color, float text_size);
struct Button init_button_image(float button_x, float button_y, float button_width, float button_height, enum ButtonType type, const char* text, CP_Color text_color, float text_size);
void render_button(struct Button button);
void update_button_state(struct Button* button, float mouse_x, float mouse_y);
void dispose_button(struct Button* button);
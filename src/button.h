/******************************************************************************
* @filename		button.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli,
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This interface declares functions and enums for an interactive
				button. Enums for the state of buttons and its type are also
				defined.
******************************************************************************/

#pragma once

#include "hittest.h"

enum ButtonState {
	BUTTON_STATE_NONE,
	BUTTON_STATE_CLICKED,
	BUTTON_STATE_HOVERED
};

enum ButtonType {
	BUTTON_TYPE_PRIMARY,
	BUTTON_TYPE_SECONDARY,
	BUTTON_TYPE_WARNING
};

struct Button {
	struct Rectangle rect;
	enum ButtonState state;
	enum ButtonType type;
	CP_Color rectColor;
	char* text;
	CP_Vector textPos;
	CP_Color textColor;
	float textSize;
};

struct Button init_button(float button_x, float button_y, float button_width, float button_height, enum ButtonType type, const char* text, CP_Color text_color, float text_size);
void render_button(struct Button button);
void update_button_state(struct Button* button, float mouse_x, float mouse_y);
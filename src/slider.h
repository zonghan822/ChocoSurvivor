/******************************************************************************
* @filename		slider.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file defines a `Slider` structure and related functions for
				initialising, updating, and rendering a slider UI element.
				It also manages the slider's state and value, with mouse 
				interaction support.
******************************************************************************/

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
	enum SliderState state;
	float value;
};

struct Slider init_slider(float width, float height, float x, float y, float diameter);
void update_slider_by_mouse(struct Slider* slider, float mouse_x, float mouse_y);
void update_slider(struct Slider* slider, float value);
void render_slider(struct Slider slider);
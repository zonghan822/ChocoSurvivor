#pragma once
#include "cprocessing.h"
#include <stdbool.h>

// Use bit-shift
enum Movement {
	STOPPED = 0,
	MOVING_UP = 1 << 0,
	MOVING_RIGHT = 1 << 1,
	MOVING_DOWN = 1 << 2,
	MOVING_LEFT = 1 << 3
};

struct Rectangle {
	CP_Vector pos;
	float width, height;
	//CP_POSITION_MODE mode;
};

struct Circle {
	CP_Vector pos;
	float diameter;
};

// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n);

// Resize width preserving aspect ratio
//void resize_by_width_preservef(float* width, float* height, float new_width);

// Resize height preserving aspect ratio
//void resize_by_height_preservef(float* width, float* height, float new_height);

float get_preserved_widthf(float width, float height, float new_height);

float get_preserved_heightf(float width, float height, float new_width);
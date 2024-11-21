#pragma once
#include "cprocessing.h"
#include <stdbool.h>

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))

// Use bit-shift
enum Movement {
	STOPPED = 0,
	MOVING_UP = 1 << 0,		// 0b_00000001 // 1	
	MOVING_RIGHT = 1 << 1,	// 0b_00000010 // 2
	MOVING_DOWN = 1 << 2,	// 0b_00000100 // 4
	MOVING_LEFT = 1 << 3	// 0b_00001000 // 8
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

//// return the normalized diagonal distance
//float get_diagonal_dist(float dist);

// Resize width preserving aspect ratio
//void resize_by_width_preservef(float* width, float* height, float new_width);

// Resize height preserving aspect ratio
//void resize_by_height_preservef(float* width, float* height, float new_height);

float get_preserved_widthf(float width, float height, float new_height);

float get_preserved_heightf(float width, float height, float new_width);

void clamp_pos_clip_rel(CP_Vector* vec, struct Rectangle entity_hitbox, struct Rectangle clip);

float resize_by_divisor(float dimension, int divisor);

void render_health_bar_rel(struct Rectangle entity, int health, int maxHealth, struct Camera cam);

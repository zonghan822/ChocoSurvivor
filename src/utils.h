#pragma once

enum Movement {
	STOP,
	MOVING_RIGHT,
	MOVING_DOWN,
	MOVING_LEFT,
	MOVING_UP,
	MOVING_UP_LEFT,
	MOVING_UP_RIGHT,
	MOVING_DOWN_LEFT,
	MOVING_DOWN_RIGHT
};

// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n);
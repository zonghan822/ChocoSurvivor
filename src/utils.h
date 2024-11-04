#pragma once

// Use bit-shift
enum Movement {
	STOPPED = 0,
	MOVING_UP = 1 << 0,
	MOVING_RIGHT = 1 << 1,
	MOVING_DOWN = 1 << 2,
	MOVING_LEFT = 1 << 3
};

// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n);
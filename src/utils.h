#pragma once

// Use bit-shift
enum Movement {
	STOPPED = 0,
	MOVING_UP = 1 << 0,		// 0b_00000001 // 1	
	MOVING_RIGHT = 1 << 1,	// 0b_00000010 // 2
	MOVING_DOWN = 1 << 2,	// 0b_00000100 // 4
	MOVING_LEFT = 1 << 3	// 0b_00001000 // 8
};

// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n);

// return the normalized diagonal distance
float get_diagonal_dist(float dist);
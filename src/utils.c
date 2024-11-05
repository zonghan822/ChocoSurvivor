#include <stdlib.h>

// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n) {
	return ((float)rand() / (float)RAND_MAX) * (n - m) + m;
}
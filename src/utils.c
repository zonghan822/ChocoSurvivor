#include <stdlib.h>
#include <math.h>

float get_diagonal_dist(float dist) {
	return (float)sqrt((dist * dist)/2);
}
// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n) {
	return ((float)rand() / (float)RAND_MAX) * (n - m) + m;
}

float get_preserved_widthf(float width, float height, float new_height) {
	float aspectRatio = width / height;
	return new_height / aspectRatio;
}

float get_preserved_heightf(float width, float height, float new_width) {
	float aspectRatio = width / height;
	return new_width / aspectRatio;
}

//void resize_by_width_preservef(float* width, float* height, float new_width) {
//	float aspectRatio = *width / *height;
//	*width = new_width;
//	*height = new_width / aspectRatio;
//}
//
//void resize_by_height_preservef(float* width, float* height, float new_height) {
//	float aspectRatio = *width / *height;
//	*height = new_height;
//	*width = new_height / aspectRatio;
//}

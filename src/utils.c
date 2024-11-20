#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "cprocessing.h"
#include "utils.h"

float get_preserved_widthf(float width, float height, float new_height) {
	float aspectRatio = width / height;
	return new_height / aspectRatio;
}

float get_preserved_heightf(float width, float height, float new_width) {
	float aspectRatio = width / height;
	return new_width / aspectRatio;
}

// Clamp pos relative to clip
void clamp_pos_clip_rel(CP_Vector* vec, struct Rectangle entity_hitbox, struct Rectangle clip) {
	float deltaX = vec->x - clip.pos.x,
		deltaY = vec->y - clip.pos.y,
		entHalfWidth = entity_hitbox.width / 2, entHalfHeight = entity_hitbox.height / 2,
		clipHalfWidth = clip.width / 2, clipHalfHeight = clip.height / 2;

    // Overlaps
    float overlapX = fabsf(deltaX) - clipHalfWidth - entHalfWidth,
        overlapY = fabsf(deltaY) - clipHalfHeight - entHalfHeight;

    // Prioritise clamping based on the largest overlap
    if (overlapX > overlapY) {
        if (fabsf(deltaX) > clipHalfWidth) {
            if (deltaX > 0) { // Right side of clip
                vec->x = CP_Math_ClampFloat(vec->x, (clip.pos.x + clipHalfWidth) + entHalfWidth, clip.pos.x + clip.width);
            }
            else { // Left side of clip
                vec->x = CP_Math_ClampFloat(vec->x, entHalfWidth, (clip.pos.x - clipHalfWidth) - entHalfWidth);
            }
        }
    }
    else {
        if (fabsf(deltaY) > clipHalfHeight) {
            // Important: Use 1/4 height, don't use half height as-is for bottom side
            if (deltaY > 0) { // Bottom side of clip
                vec->y = CP_Math_ClampFloat(vec->y, (clip.pos.y + clipHalfHeight) + (entHalfHeight / 2), clip.pos.y + clip.height);
            }
            else { // Top side of clip
                vec->y = CP_Math_ClampFloat(vec->y, entHalfHeight, (clip.pos.y - clipHalfHeight) - entHalfHeight);
            }
        }
    }
}

float resize_by_divisor(float dimension, int divisor) {
    int intDimension = (int)dimension;
    if (intDimension % divisor != 0) {
        // Round up to the nearest multiple of divisor
        return (float)((intDimension / divisor + 1) * divisor);
    }
    return dimension;
}


//float get_diagonal_dist(float dist) {
//	return (float)sqrt((dist * dist)/2);
//}
// 
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

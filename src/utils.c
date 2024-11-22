#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "cprocessing.h"
#include "utils.h"
#include "camera.h"

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

void render_health_bar_rel(struct Rectangle entity, int health, int maxHealth) {
    // Set position of health bar
    float health_bar_w = entity.width;
    float health_bar_h = 15.f;
    float health_bar_x = entity.pos.x - (health_bar_w / 2);
    float health_bar_y = (entity.pos.y) - (entity.height / 2) - 20.f;

    CP_Settings_RectMode(CP_POSITION_CORNER);

    // Draw health bar rect
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Graphics_DrawRect(health_bar_x, health_bar_y, health_bar_w, health_bar_h);

    float health_gap_x = health_bar_x + ((float)health / (float)maxHealth) * health_bar_w;
    float health_gap_w = health_bar_w * (1.f - ((float)health / (float)maxHealth));

    CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
    CP_Graphics_DrawRect(health_gap_x, health_bar_y, health_gap_w, health_bar_h);
    CP_Settings_RectMode(CP_POSITION_CENTER);
}

void render_cursor(CP_Image cursor) {
    CP_Settings_ImageMode(CP_POSITION_CORNER);
    float cursorWidth = 60.f;
    float mouseX = CP_Input_GetMouseX(),
        mouseY = CP_Input_GetMouseY();
    // 20.f is to correct its draw position with the actual cursor position
    CP_Image_Draw(cursor, mouseX - 20.f, mouseY, cursorWidth,
        get_preserved_heightf(CP_Image_GetWidth(cursor), CP_Image_GetHeight(cursor), cursorWidth), 255);
    CP_Settings_ImageMode(CP_POSITION_CENTER);
}
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

void render_health_bar_rel(struct Rectangle entity, int health, int maxHealth, struct Camera cam) {

    // get relative position after camera transformation
    CP_Vector vec = get_cam_pos_rel(entity.pos, cam);

    // Set position of health bar
    float health_bar_w = entity.width;
    float health_bar_h = 15.f;
    float health_bar_x = vec.x - (health_bar_w / 2);
    float health_bar_y = (vec.y) - (entity.height / 2) - 20.f;

    CP_Settings_RectMode(CP_POSITION_CORNER);

    // Draw health bar rect
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Graphics_DrawRect(health_bar_x, health_bar_y, health_bar_w, health_bar_h);

    float health_gap_x = health_bar_x + ((float)health / (float)maxHealth) * health_bar_w;
    float health_gap_w = health_bar_w * (1.f - ((float)health / (float)maxHealth));

    CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
    CP_Graphics_DrawRect(health_gap_x, health_bar_y, health_gap_w, health_bar_h);
}

void render_health_heart_rel(struct Rectangle entity, int health, int maxHealth, struct Camera cam) {
    
    CP_Image img = CP_Image_Load("Assets/heart.png");

    // get relative position after camera transformation
    CP_Vector vec = get_cam_pos_rel(entity.pos, cam);

    // Set position of the first health icon
    float health_icon_w = 45.f;
    float health_icon_h = get_preserved_heightf(CP_Image_GetWidth(img), CP_Image_GetHeight(img), health_icon_w);
    float health_icon_y = (vec.y) - (entity.height / 2) - (health_icon_h/2)- 10.f;
    float health_icon_gap = health_icon_w / 2;
    for (int i = 0; i < health; i++) {
        float health_icon_x = (vec.x - (entity.width / maxHealth)) + (i * (entity.width / maxHealth));
        CP_Image_Draw(img, health_icon_x, health_icon_y, health_icon_w, health_icon_h, 255);
    }
}
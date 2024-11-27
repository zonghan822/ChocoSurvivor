/******************************************************************************
* @filename		utils.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file contains utility functions, including functions to
                resize dimensions while preserving aspect ratio, clamp entity
                movement within a boundary, and adjust object positions. It
                also provides a health bar renderer that adjusts based on the
                entity's health, and a custom cursor renderer that scales and
                positions a given cursor image. Additionally, it includes a 
                helper function to round a dimension up to the nearest multiple
                of a divisor (mainly for Nine-Patch Rect or 9 Slicing).
******************************************************************************/

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include "cprocessing.h"
#include "utils.h"
#include "camera.h"
#include "hittest.h"

float get_preserved_widthf(float width, float height, float new_height) {
	float aspectRatio = width / height;
	return new_height / aspectRatio;
}

float get_preserved_heightf(float width, float height, float new_width) {
	float aspectRatio = width / height;
	return new_width / aspectRatio;
}

// Clamp pos relative to clip
void clamp_pos_clip_rel(struct Rectangle entity_hitbox, CP_Vector* movement, struct Rectangle clip_hitbox) {
    float entHalfWidth = entity_hitbox.width / 2.f,
        entHalfHeight = entity_hitbox.height / 2.f,
        clipHalfWidth = clip_hitbox.width / 2.f,
        clipHalfHeight = clip_hitbox.height / 2.f;

    struct Rectangle temp = entity_hitbox;

    // Check x-axis movement
    temp.pos.x += movement->x;
    if (are_rects_intersecting(temp, clip_hitbox))
        movement->x = 0;

    // Y-axis now
    temp.pos.y += movement->y;
    if (are_rects_intersecting(temp, clip_hitbox))
        movement->y = 0;
}

float resize_by_divisor(float dimension, int divisor) {
    int intDimension = (int)dimension;
    if (intDimension % divisor != 0) {
        // Round up to the nearest multiple of divisor
        return (float)((intDimension / divisor + 1) * divisor);
    }
    return dimension;
}

// Made by Law Zong Han
void render_health_bar(struct Rectangle entity, int health, int maxHealth) {
    // Set position of health bar
    float healthBarW = entity.width;
    float healthBarH = 15.f;
    float healthBarX = entity.pos.x - (healthBarW / 2);
    float healthBarY = (entity.pos.y) - (entity.height / 2) - 20.f;

    CP_Settings_RectMode(CP_POSITION_CORNER);

    // Draw health bar rect
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Graphics_DrawRect(healthBarX, healthBarY, healthBarW, healthBarH);

    float healthGapX = healthBarX + ((float)health / (float)maxHealth) * healthBarW;
    float healthGapW = healthBarW * (1.f - ((float)health / (float)maxHealth));

    CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
    CP_Graphics_DrawRect(healthGapX, healthBarY, healthGapW, healthBarH);
    CP_Settings_RectMode(CP_POSITION_CENTER);
}

void pop_int_array(int* arr, int *size, int element) {
    for (int i = 0; i < *size; i++) {
        if (arr[i] == element) {
            for (int j = i; j < (*size)-1; j++) {
                int tmp = arr[j];
                arr[j] = arr[j+1];
                arr[j + 1] = tmp;
            }
            (*size)--;
            break;
        }
    }
}

void render_cursor(CP_Image cursor) {
    CP_Settings_ImageMode(CP_POSITION_CORNER);
    float cursorWidth = 60.f;
    float mouseX = CP_Input_GetMouseX(),
        mouseY = CP_Input_GetMouseY();
    // 20.f is to correct its draw position with the actual cursor position
    float cursorImgWidth = ((float)CP_Image_GetWidth(cursor)),
        cursorImgHeight = ((float)CP_Image_GetHeight(cursor));
    CP_Image_Draw(cursor, mouseX - 20.f, mouseY, cursorWidth,
        get_preserved_heightf(cursorImgWidth, cursorImgHeight, cursorWidth), 255);
    CP_Settings_ImageMode(CP_POSITION_CENTER);
}
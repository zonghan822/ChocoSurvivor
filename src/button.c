/******************************************************************************
* @filename		button.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli,
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file provides function definitions to handle logic for
                button rendering, and updating its button states.

                The 9-Slicing or Nine-Patch Rect function is outdated. Check
                progressbar for updated version.
******************************************************************************/

#include "button.h"
#include "hittest.h"
#include "audio.h"
#include "cprocessing.h"
#include "utils.h"
#include "gui.h"

#include <stdio.h>

struct Button init_button(float button_x, float button_y, float button_width, float button_height, enum ButtonType type, const char* text, CP_Color text_color, float text_size) {
    float textX = button_x,
        textY = button_y + 2.f; // padding

    struct Rectangle rect = {
        .width = resize_by_divisor(button_width, 4),
        .height = resize_by_divisor(button_height, 4),
        .pos = CP_Vector_Set(button_x, button_y)
    };

    struct Button button = {
        .rect = rect,
        .state = BUTTON_STATE_NONE,
        .type = type,
        .text = text,
        .textColor = text_color,
        .textPos = CP_Vector_Set(textX, textY),
        .textSize = text_size
    };

    return button;
}

// This is outdated function, but it still works
// Check slider.c or progressbar.c for updated function and comments
void render_button(struct Button button) {
    // Nine-Patch Rect / 9 Slicing
    float uStart = 51.f, vStart;

    switch (button.type) {
    case BUTTON_TYPE_WARNING:
        vStart = 2.f;
        break;
    case BUTTON_TYPE_SECONDARY:
        vStart = 36.f;
        break;
    case BUTTON_TYPE_PRIMARY:
    default:
        vStart = 19.f;
        break;
    }

    float leftWidth, rightWidth, topHeight, bottomHeight;
    leftWidth = rightWidth = topHeight = bottomHeight = 10.0f;

    // Center stretchable area � remaining part after accounting for corners and edges
    float centerWidth = button.rect.width - (leftWidth + rightWidth),
        centerHeight = button.rect.height - (topHeight + bottomHeight);

    float uCoords[4] = { 0.0f, 4.0f, 12.0f, 16.0f },  // Left, center, right bounds for U (with respect to 16px width)
        vCoords[4] = { 0.0f, 4.0f, 8.0f, 12.0f };     // Top, center, bottom bounds for V (with respect to 12px height)

    float xPos[4] = {
        button.rect.pos.x - button.rect.width / 2.0f,               // Left
        button.rect.pos.x - button.rect.width / 2.0f + leftWidth,   // Center start
        button.rect.pos.x + button.rect.width / 2.0f - rightWidth,  // Center end (right)
        button.rect.pos.x + button.rect.width / 2.0f                // Right
    };

    float yPos[4] = {
        button.rect.pos.y - button.rect.height / 2.0f,                // Top
        button.rect.pos.y - button.rect.height / 2.0f + topHeight,    // Center start
        button.rect.pos.y + button.rect.height / 2.0f - bottomHeight, // Center end (bottom)
        button.rect.pos.y + button.rect.height / 2.0f                 // Bottom
    };

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Determine size to draw for this patch
            float patchWidth = (col == 1) ? centerWidth : leftWidth;   // Center column stretches
            float patchHeight = (row == 1) ? centerHeight : topHeight; // Center row stretches

            // Calculate draw position (the center of each patch,
            // since the CP_Image_DrawSubImage() function centers it)
            float drawX = (xPos[col] + xPos[col + 1]) / 2.0f,
                drawY = (yPos[row] + yPos[row + 1]) / 2.0f;

            // Calculate the UV coordinates within the sprite for this patch
            float u0 = uStart + uCoords[col],
                v0 = vStart + vCoords[row],
                u1 = uStart + uCoords[col + 1],
                v1 = vStart + vCoords[row + 1];

            // Draw the patch
            CP_Image_DrawSubImage(tilemap, drawX, drawY, patchWidth, patchHeight, u0, v0, u1, v1, 255);
        }
    }

    CP_Settings_TextSize(button.textSize);
    CP_Settings_Fill(button.textColor);
    CP_Font_DrawText(button.text, button.textPos.x, button.textPos.y);
}

void update_button_state(struct Button* button, float mouse_x, float mouse_y) {
    if (is_mouse_on_rect(button->rect, mouse_x, mouse_y)) {
        // Trigger once
        if (button->state != BUTTON_STATE_HOVERED && button->state != BUTTON_STATE_CLICKED) {
            play_sound(SOUND_UI_BUTTON_HOVER);
            // Anything else
        }

        button->state = BUTTON_STATE_HOVERED;
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            switch (button->type) {
            default:
            case BUTTON_TYPE_SECONDARY:
                play_sound(SOUND_UI_BUTTON_CONFIRM_CLICK);
                break;
            case BUTTON_TYPE_WARNING:
                play_sound(SOUND_UI_BUTTON_BACK_CLICK);
                break;
            }
            button->state = BUTTON_STATE_CLICKED;
        }
    }
    else
        button->state = BUTTON_STATE_NONE;
}
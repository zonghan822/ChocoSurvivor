/******************************************************************************
* @filename		slider.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This implementation file defines functions for initialising,
                updating, and rendering a slider UI element, including support
                for mouse interaction and visual feedback. Nine-patch slicing
                is used for customisable `Slider` graphics.
******************************************************************************/

#include "hittest.h"
#include "cprocessing.h"
#include "slider.h"
#include <math.h>
#include "gui.h"

struct Slider init_slider(float width, float height, float x, float y, float diameter) {
    float w = resize_by_divisor(width, 4),
        h = resize_by_divisor(height, 4);

    struct Slider slider = {
        .track = {
            .width = w,
            .height = h,
            .pos = CP_Vector_Set(x, y)
        },
        .thumb = {
            .pos = CP_Vector_Set(x - w / 2.f, y - h / 2.f),
            .diameter = diameter
        },
        .value = 0,
        .state = 0
    };

    return slider;
}

void update_slider(struct Slider* slider, float value) {
    value = CP_Math_ClampFloat(value, 0, 1.f);

    slider->value = value;
    slider->thumb.pos.x = slider->track.pos.x - slider->track.width / 2.f + value * slider->track.width;
}

void update_slider_by_mouse(struct Slider* slider, float mouse_x, float mouse_y) {
    if (is_mouse_on_circle(slider->thumb, mouse_x, mouse_y) && CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
        slider->state = SLIDER_STATE_HELD;

    if (slider->state == SLIDER_STATE_HELD)
    {
        float xLeft = slider->track.pos.x - slider->track.width / 2.f,
            xRight = xLeft + slider->track.width;
        slider->thumb.pos.x = CP_Math_ClampFloat(mouse_x, xLeft, xRight);

        // Get normalized value
        float v = fabsf((xLeft - slider->thumb.pos.x) / slider->track.width);
        slider->value = CP_Math_ClampFloat(v, 0, 1.f);
    }

    if (CP_Input_MouseReleased(MOUSE_BUTTON_LEFT))
        slider->state = SLIDER_STATE_NONE;
}

// Progressbar.c is the most updated version of 9 slicing
// This version does not take into account that the leftmost
// edge is still rendered despite value is 0 (thumb is hiding it)
void render_slider(struct Slider slider) {
    // Comments are added to explain as much as possible

    // Nine-Patch Rect / 9 Slicing

    // UV start coords in tilemap
    float baseUStart = 2.f, baseVStart = 53.f,
        trackUStart = 71.f, trackVStart = 54.f,
        thumbUStart = 1.f, thumbVStart = 18.f;

    // Render the base first
    // Render the track second
    // Render the thumb last

#pragma region Base

    // uCoords and vCoords computed for the Base, similar to the Track
    float baseUCoords[4] = { 0.f, 3.f, 9.f, 12.f };  // 0-3, 3-9, 9-12
    float baseVCoords[4] = { 0.f, 3.f, 9.f, 12.f };  // 0-3, 3-9, 9-12

    // Define fixed edge sizes
    const float baseEdgeWidth = 3.f;
    const float baseEdgeHeight = 3.f;

    // Base position slices
    // Edges are like the offset
    float xBasePos[4] = {
        slider.track.pos.x - slider.track.width / 2.f,                           // Left X
        slider.track.pos.x - slider.track.width / 2.f + baseEdgeWidth,           // Center start
        slider.track.pos.x + slider.track.width / 2.f - baseEdgeWidth,           // Center end
        slider.track.pos.x + slider.track.width / 2.f                            // Right X
    };

    float yBasePos[4] = {
        slider.track.pos.y - slider.track.height / 2.f,                         // Top Y
        slider.track.pos.y - slider.track.height / 2.f + baseEdgeHeight,        // Center start
        slider.track.pos.y + slider.track.height / 2.f - baseEdgeHeight,        // Center end
        slider.track.pos.y + slider.track.height / 2.f                          // Bottom Y
    };

    // Render the base (3x3)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Calculate patch dimensions
            float patchWidth = (col == 1) ? (slider.track.width - 2 * baseEdgeWidth) : baseEdgeWidth;
            float patchHeight = (row == 1) ? (slider.track.height - 2 * baseEdgeHeight) : baseEdgeHeight;

            // Calculate the draw position (center of each patch)
            float drawX = (xBasePos[col] + xBasePos[col + 1]) / 2.f;
            float drawY = (yBasePos[row] + yBasePos[row + 1]) / 2.f;

            // UV coordinates for the base patch
            float u0 = baseUStart + baseUCoords[col];
            float v0 = baseVStart + baseVCoords[row];
            float u1 = baseUStart + baseUCoords[col + 1];
            float v1 = baseVStart + baseVCoords[row + 1];

            // Render this patch of the base
            CP_Image_DrawSubImage(tilemap,
                drawX, drawY,
                patchWidth, patchHeight,
                u0, v0, u1, v1,
                255);
        }
    }
#pragma endregion

#pragma region Track
    slider.track.width -= 4.f;
    slider.track.height -= 4.f;

    // Define fixed edge sizes based on UV slicing (3px each side)
    // Then, adjust the track size to account for edges
    const float trackEdgeWidth = 4.f;
    const float trackEdgeHeight = 4.f;
    float innerTrackHeight = fabsf(slider.track.height - 2 * trackEdgeHeight);

    // Fill length
    float fillLength = slider.value * slider.track.width;

    // uCoords and vCoords arrays for proper slicing of the track texture
    float uCoords[4] = { 0.f, 3.f, 7.f, 10.f };  // 0-3, 3-7, 7-10
    float vCoords[4] = { 0.f, 3.f, 7.f, 10.f };  // 0-3, 3-7, 7-10

    // Left most of X coordinates
    float leftMostX = slider.track.pos.x - slider.track.width / 2.f;

    // Define track position slices
    // [---------]
    // Leftmost                  [
    // Center start (left inner)  --...
    // Center end (right inner)        ...--
    // Rightmost                            ]
    float xTrackPos[4] = {
        leftMostX,
        leftMostX + trackEdgeWidth,
        leftMostX + fillLength - trackEdgeWidth,
        leftMostX + fillLength + trackEdgeWidth
    };

    float yTrackPos[4] = {
        slider.track.pos.y - slider.track.height / 2.f,                                 // Top Y
        slider.track.pos.y - slider.track.height / 2.f + trackEdgeHeight,               // Center start 
        slider.track.pos.y + slider.track.height / 2.f - trackEdgeHeight,               // Center end
        slider.track.pos.y + slider.track.height / 2.f                                  // Bottom Y
    };

    // Render the 3x3 grid for the track
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Calculate patch dimensions based on position
            float patchWidth = (col == 1) ? fillLength : trackEdgeWidth;
            float patchHeight = (row == 1) ? innerTrackHeight : trackEdgeHeight;

            // Calculate the draw position (center of each patch)
            float drawX = (xTrackPos[col] + xTrackPos[col + 1]) / 2.f;
            float drawY = (yTrackPos[row] + yTrackPos[row + 1]) / 2.f;

            // UV coordinates for the track patch
            float u0 = trackUStart + uCoords[col];
            float v0 = trackVStart + vCoords[row];
            float u1 = trackUStart + uCoords[col + 1];
            float v1 = trackVStart + vCoords[row + 1];

            // Render this patch of the track
            CP_Image_DrawSubImage(tilemap,
                drawX, drawY,
                patchWidth, patchHeight,
                u0, v0, u1, v1,
                255);
        }
    }

#pragma endregion

#pragma region Thumb
    CP_Image_DrawSubImage(tilemap,
        slider.thumb.pos.x, slider.track.pos.y,             // Thumb position according to filled track and value
        slider.thumb.diameter, slider.thumb.diameter,       // Use the circle diameter
        thumbUStart, thumbVStart,
        thumbUStart + 14.f, thumbVStart + 14.f,             // 14 is the sprite size
        255);
#pragma endregion
}
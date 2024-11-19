#include "hittest.h"
#include "cprocessing.h"
#include "slider.h"
#include <math.h>

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
        .tileMap = CP_Image_Load("Assets/GUI/tilemap.png"),
    };

    return slider;
}

void update_slider(struct Slider* slider, float value) {
    // Min and max values
    float start = slider->track.pos.x - slider->track.width / 2.f,
        end = slider->track.pos.y - slider->track.height / 2.f;

    // Normalize the values
    float normalizedMin = 0 / (end - start);
    float normalizedMax = (end - start) / (end - start);

    value = CP_Math_ClampFloat(value, normalizedMin, normalizedMax);

    // Denormalize(?) it
    float thumbPos = value * (end - start) + start;

    slider->thumb.pos.x = thumbPos;
}

void render_slider(struct Slider slider) {
    float uBaseStart = 2.f, vBaseStart = 53.f,
        uTrackStart = 71.f, vTrackStart = 54.f;

    float leftWidth, rightWidth, topHeight, bottomHeight;
    leftWidth = rightWidth = topHeight = bottomHeight = 4.0f;

    // Center stretchable area — remaining part after accounting for corners and edges
    float centerWidth = slider.track.width - (leftWidth + rightWidth),
        centerHeight = slider.track.height - (topHeight + bottomHeight);

    float uBaseCoords[4] = { 0.0f, 4.0f, 8.0f, 12.0f },  // Left, center, right bounds for U
        vBaseCoords[4] = { 0.0f, 4.0f, 8.0f, 12.0f },    // Top, center, bottom bounds for V
        uTrackCoords[4] = { 0.0f, 3.0f, 7.0f, 10.f },
        vTrackCoords[4] = { 0.0f, 3.0f, 7.0f, 10.f };

    float xBasePos[4] = {
        slider.track.pos.x - slider.track.width / 2.0f,               // Left
        slider.track.pos.x - slider.track.width / 2.0f + leftWidth,   // Center start
        slider.track.pos.x + slider.track.width / 2.0f - rightWidth,  // Center end (right)
        slider.track.pos.x + slider.track.width / 2.0f                // Right
    },
        xTrackPos[4] = {
        slider.thumb.pos.x - (slider.track.width / 2.0f) - 1,
        slider.thumb.pos.x - (slider.track.width / 2.0f + leftWidth) - 1,
        slider.thumb.pos.x + (slider.track.width / 2.0f - rightWidth) - 1,
        slider.thumb.pos.x + (slider.track.width / 2.0f) - 1
    };

    float yBasePos[4] = {
        slider.track.pos.y - slider.track.height / 2.0f,                // Top
        slider.track.pos.y - slider.track.height / 2.0f + topHeight,    // Center start
        slider.track.pos.y + slider.track.height / 2.0f - bottomHeight, // Center end (bottom)
        slider.track.pos.y + slider.track.height / 2.0f                 // Bottom
    },
        yTrackPos[4] = {
        slider.thumb.pos.y - (slider.track.height / 2.0f) - 1,
        slider.thumb.pos.y - (slider.track.height / 2.0f + topHeight) - 1,
        slider.thumb.pos.y + (slider.track.height / 2.0f - bottomHeight) - 1,
        slider.thumb.pos.y + (slider.track.height / 2.0f) - 1
    };

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Determine size to draw for this patch
            float patchWidth = (col == 1) ? centerWidth : leftWidth;   // Center column stretches
            float patchHeight = (row == 1) ? centerHeight : topHeight; // Center row stretches

            // Calculate draw position (the center of each patch,
            // since the CP_Image_DrawSubImage() function centers it)
            float drawX = (xBasePos[col] + xBasePos[col + 1]) / 2.0f,
                drawY = (yBasePos[row] + yBasePos[row + 1]) / 2.0f;

            // Calculate the UV coordinates within the sprite for this patch
            float u0 = uBaseStart + uBaseCoords[col],
                v0 = vBaseStart + vBaseCoords[row],
                u1 = uBaseStart + uBaseCoords[col + 1],
                v1 = vBaseStart + vBaseCoords[row + 1];

            // Draw the patch
            CP_Image_DrawSubImage(slider.tileMap, drawX, drawY, patchWidth, patchHeight, u0, v0, u1, v1, 255);
        }
    }

    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            float patchWidth = (col == 1) ? centerWidth : leftWidth;
            float patchHeight = (row == 1) ? centerHeight : topHeight;

            float drawX = (xTrackPos[col] + xTrackPos[col + 1]) / 2.0f,
                drawY = (yTrackPos[row] + yTrackPos[row + 1]) / 2.0f;

            float u0 = uTrackStart + uTrackCoords[col],
                v0 = vTrackStart + vTrackCoords[row],
                u1 = uTrackStart + uTrackCoords[col + 1],
                v1 = vTrackStart + vTrackCoords[row + 1];

            CP_Image_DrawSubImage(slider.tileMap, drawX, drawY, patchWidth, patchHeight, u0, v0, u1, v1, 255);
        }
    }
}

void dispose_slider(struct Slider* slider) {
    CP_Image_Free(slider->tileMap);
}
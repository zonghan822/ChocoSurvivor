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
        .value = 0
    };

    return slider;
}

void update_slider(struct Slider* slider, float value) {
    value = CP_Math_ClampFloat(value, 0, 1.f);

    slider->value = value;
    slider->thumb.pos.x = slider->track.pos.x - slider->track.width / 2.f + value * slider->track.width;
}

void update_slider_by_mouse(struct Slider* slider, float mouse_x, float mouse_y) {
    if (is_mouse_on_circle(slider->thumb, mouse_x, mouse_y) && CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
        float xLeft = slider->track.pos.x - slider->track.width / 2.f,
            xRight = xLeft + slider->track.width;
        slider->thumb.pos.x = CP_Math_ClampFloat(mouse_x, xLeft, xRight);

        // Get normalized value
        float v = fabs((xLeft - slider->thumb.pos.x) / slider->track.width);
        slider->value = CP_Math_ClampFloat(v, 0, 1.f);
    }
}

void render_slider(struct Slider slider) {
    // Comments are added to explain as much as possible
    // because it can get confusing

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
    float baseUCoords[4] = { 0.f, 2.f, 10.f, 12.f };  // UV bounds for base, using 12px total width
    float baseVCoords[4] = { 0.f, 2.f, 10.f, 12.f };  // UV bounds for base, using 12px total height

    float xBasePos[4] = {
        slider.track.pos.x - slider.track.width / 2.f,                  // Left X
        slider.track.pos.x - slider.track.width / 2.f + 2.f,            // Center start (left inner)
        slider.track.pos.x + slider.track.width / 2.f - 2.f,            // Center end (right inner)
        slider.track.pos.x + slider.track.width / 2.f                   // Right X
    };

    float yBasePos[4] = {
        slider.track.pos.y - slider.track.height / 2.f,                 // Top Y
        slider.track.pos.y - slider.track.height / 2.f + 2.f,           // Center start 
        slider.track.pos.y + slider.track.height / 2.f - 2.f,           // Center end
        slider.track.pos.y + slider.track.height / 2.f                  // Bottom Y
    };

    // Render the base (3x3)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Calculate patch dimensions
            float patchWidth = (col == 1) ? slider.track.width - 4.f : 2.f;   // Center stretches
            float patchHeight = (row == 1) ? slider.track.height - 4.f : 2.f; // Center stretches

            // Calculate the draw position (center of each patch)
            float drawX = (xBasePos[col] + xBasePos[col + 1]) / 2.f;
            float drawY = (yBasePos[row] + yBasePos[row + 1]) / 2.f;

            // UV coordinates for the base patch
            float u0 = baseUStart + baseUCoords[col];
            float v0 = baseVStart + baseVCoords[row];
            float u1 = baseUStart + baseUCoords[col + 1];
            float v1 = baseVStart + baseVCoords[row + 1];

            CP_Image_DrawSubImage(slider.tileMap,
                drawX, drawY,
                patchWidth, patchHeight,
                u0, v0, u1, v1,
                255);
        }
    }
#pragma endregion

#pragma region Track

    // Adjust the track size to shrink by 2px per side
    slider.track.width -= 4.f;
    slider.track.height -= 4.f;

    // uCoords and vCoords arrays for proper slicing of the track texture
    float uCoords[4] = { 0.f, 2.f, 8.f, 10.f };  // Using 10px source width
    float vCoords[4] = { 0.f, 2.f, 8.f, 10.f };  // Using 10px source height

    // Slider configuration: Split the track using Nine-Patch coordinates
    float xTrackPos[4] = {
        slider.track.pos.x - slider.track.width / 2.f,              // Left X
        slider.track.pos.x - slider.track.width / 2.f + 2.f,       // Center start (left inner)
        slider.track.pos.x + slider.track.width / 2.f - 2.f,       // Center end (right inner)
        slider.track.pos.x + slider.track.width / 2.f               // Right X
    };

    float yTrackPos[4] = {
        slider.track.pos.y - slider.track.height / 2.f,             // Top Y
        slider.track.pos.y - slider.track.height / 2.f + 2.f,      // Center start 
        slider.track.pos.y + slider.track.height / 2.f - 2.f,      // Center end
        slider.track.pos.y + slider.track.height / 2.f              // Bottom Y
    };

    // Render the 3x3 grid for the track as per Nine-Patch technique
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Calculate patch dimensions
            float patchWidth = (col == 1) ? slider.track.width - 4.f : 2.f;   // Center width stretches
            float patchHeight = (row == 1) ? slider.track.height - 4.f : 2.f; // Center height stretches

            // Calculate the draw position (center of each patch)
            float drawX = (xTrackPos[col] + xTrackPos[col + 1]) / 2.f;
            float drawY = (yTrackPos[row] + yTrackPos[row + 1]) / 2.f;

            // UV coordinates for the track patch
            float u0 = trackUStart + uCoords[col];
            float v0 = trackVStart + vCoords[row];
            float u1 = trackUStart + uCoords[col + 1];
            float v1 = trackVStart + vCoords[row + 1];

            // Render this patch of the track
            CP_Image_DrawSubImage(slider.tileMap,
                drawX, drawY,
                patchWidth, patchHeight,
                u0, v0, u1, v1,
                255);
        }
    }
#pragma endregion

#pragma region Thumb
    // Thumb pos based on value
    CP_Image_DrawSubImage(slider.tileMap,
        slider.thumb.pos.x, slider.track.pos.y, // Thumb position according to filled track and value
        slider.thumb.diameter, slider.thumb.diameter, // Use the circle diameter
        thumbUStart, thumbVStart, thumbUStart + 14.f, thumbVStart + 14.f, // 14 is the sprite size
        255);
#pragma endregion
}

void dispose_slider(struct Slider* slider) {
    CP_Image_Free(slider->tileMap);
}
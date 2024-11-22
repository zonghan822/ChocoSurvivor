#include "hittest.h"
#include "cprocessing.h"
#include "progressbar.h"
#include <math.h>
#include "gui.h"

struct ProgressBar init_progressbar(float width, float height, float x, float y, CP_Color baseColor, enum ProgressBarColor barColor) {
    float w = resize_by_divisor(width, 4),
        h = resize_by_divisor(height, 4);

    struct ProgressBar progressBar = {
        .track = {
            .width = w,
            .height = h,
            .pos = CP_Vector_Set(x, y)
        },
        .value = 0,
        .barColor = barColor,
        .baseColor = baseColor
    };

    return progressBar;
}

void update_progressbar(struct ProgressBar* progressbar, float value, float max) {
    float normalizedValue = value / max;
    value = CP_Math_ClampFloat(normalizedValue, 0, 1.f);

    progressbar->value = value;
}

void render_progressbar_custom(float width, float height, float x, float y, CP_Color baseColor, enum ProgressBarColor barColor, float value, float max) {
    struct ProgressBar progBar = init_progressbar(width, height, x, y, baseColor, barColor);
    update_progressbar(&progBar, value, max);
    render_progressbar(progBar);
}

void render_progressbar(struct ProgressBar progressBar) {
    // This is based on `render_slider()`

    // UV start coords in tilemap
    float baseUStart = 2.f, baseVStart = 53.f,
        trackUStart = 37.f, trackVStart = 54.f;

    switch (progressBar.barColor) {
    default:
    case PROGRESSBAR_COLOR_RED:
        trackUStart = 37.f;
        break;
    case PROGRESSBAR_COLOR_BLUE:
        trackUStart = 71.f;
        break;
    case PROGRESSBAR_COLOR_GREEN:
        trackUStart = 105.f;
        break;
    }

#pragma region Base
    // uCoords and vCoords computed for the Base, similar to the Track
    float baseUCoords[4] = { 0.f, 3.f, 9.f, 12.f };  // 0-3, 3-9, 9-12
    float baseVCoords[4] = { 0.f, 3.f, 9.f, 12.f };  // 0-3, 3-9, 9-12

    // Define fixed edge sizes based on UV slicing
    const float baseEdgeWidth = 3.f;
    const float baseEdgeHeight = 3.f;

    // Base position slices
    float xBasePos[4] = {
        progressBar.track.pos.x - progressBar.track.width / 2.f,                           // Left X
        progressBar.track.pos.x - progressBar.track.width / 2.f + baseEdgeWidth,           // Center start
        progressBar.track.pos.x + progressBar.track.width / 2.f - baseEdgeWidth,           // Center end
        progressBar.track.pos.x + progressBar.track.width / 2.f                            // Right X
    };

    float yBasePos[4] = {
        progressBar.track.pos.y - progressBar.track.height / 2.f,                         // Top Y
        progressBar.track.pos.y - progressBar.track.height / 2.f + baseEdgeHeight,        // Center start
        progressBar.track.pos.y + progressBar.track.height / 2.f - baseEdgeHeight,        // Center end
        progressBar.track.pos.y + progressBar.track.height / 2.f                          // Bottom Y
    };

    // Draw base color
    CP_Settings_StrokeWeight(0);
    CP_Settings_Fill(progressBar.baseColor);
    CP_Graphics_DrawRect(progressBar.track.pos.x, progressBar.track.pos.y,
        progressBar.track.width - 4.f, progressBar.track.height - 4.f);
    CP_Settings_StrokeWeight(3.f); // Reset

    // Render the base (3x3)
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            // Calculate patch dimensions
            float patchWidth = (col == 1) ? (progressBar.track.width - 2 * baseEdgeWidth) : baseEdgeWidth;
            float patchHeight = (row == 1) ? (progressBar.track.height - 2 * baseEdgeHeight) : baseEdgeHeight;

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
    progressBar.track.width -= 4.f;
    progressBar.track.height -= 4.f;

    // Define fixed edge sizes based on UV slicing (3px each side)
    // Then, adjust the track size to account for edges
    const float trackEdgeWidth = 4.f;
    const float trackEdgeHeight = 4.f;
    float innerTrackHeight = fabsf(progressBar.track.height - 2 * trackEdgeHeight);

    // Fill length
    float fillLength = progressBar.value * progressBar.track.width;

    // uCoords and vCoords arrays for proper slicing of the track texture
    float uCoords[4] = { 0.f, 3.f, 7.f, 10.f };  // 0-3, 3-7, 7-10
    float vCoords[4] = { 0.f, 3.f, 7.f, 10.f };  // 0-3, 3-7, 7-10

    // Left most of X coordinates
    float leftMostX = progressBar.track.pos.x - progressBar.track.width / 2.f;

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
        progressBar.track.pos.y - progressBar.track.height / 2.f,                                 // Top Y
        progressBar.track.pos.y - progressBar.track.height / 2.f + trackEdgeHeight,               // Center start 
        progressBar.track.pos.y + progressBar.track.height / 2.f - trackEdgeHeight,               // Center end
        progressBar.track.pos.y + progressBar.track.height / 2.f                                  // Bottom Y
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
}

void dispose_progressbar(struct ProgressBar* progressBar) {
    CP_Image_Free(tilemap);
}
/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
* @filename		panel.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli,
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file has definitions for rendering panels through 9-Slicing
                or Nine-Patch Rect.

                The 9-Slicing or Nine-Patch Rect function is outdated. Check
                progressbar for updated version.
******************************************************************************/

#include "cprocessing.h"
#include "hittest.h"
#include "panel.h"
#include "gui.h"

// This is outdated function, but it still works
// Check slider.c or progressbar.c for updated function and comments
void render_panel(float width, float height, float x, float y) {
    width = resize_by_divisor(width, 4);
    height = resize_by_divisor(height, 4);
    float uStart = 33.f, vStart = 0;

    float leftWidth, rightWidth, topHeight, bottomHeight;
    leftWidth = rightWidth = topHeight = bottomHeight = 16.0f;

    // Center stretchable area — remaining part after accounting for corners and edges
    float centerWidth = width - (leftWidth + rightWidth),
        centerHeight = height - (topHeight + bottomHeight);

    float uCoords[4] = { 0.0f, 8.0f, 24.0f, 32.0f },  // Left, center, right bounds for U
        vCoords[4] = { 0.0f, 8.0f, 24.0f, 32.0f };    // Top, center, bottom bounds for V

    float xPos[4] = {
        x - width / 2.0f,               // Left
        x - width / 2.0f + leftWidth,   // Center start
        x + width / 2.0f - rightWidth,  // Center end (right)
        x + width / 2.0f                // Right
    };

    float yPos[4] = {
        y - height / 2.0f,                // Top
        y - height / 2.0f + topHeight,    // Center start
        y + height / 2.0f - bottomHeight, // Center end (bottom)
        y + height / 2.0f                 // Bottom
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
            CP_Image_DrawSubImage(tilemapLarge, drawX, drawY, patchWidth, patchHeight, u0, v0, u1, v1, 255);
        }
    }
}
#include <stdio.h>
#include <stdbool.h>

#include "cprocessing.h"
#include "utils.h"
#include "mainmenu.h"
#include "player.h"
#include "enemy.h"
#include "boss.h"
#include "flicker.h"

// Constants for flickering effect
static const float flickerInterval = 0.1f;
static const float flickerDuration = 1.5f;

// Define flicker states for player and boss
struct FlickerState playerFlickerState = { false, 0.0f, 0.0f, true };
struct FlickerState bossFlickerState = { false, 0.0f, 0.0f, true };

void flicker(struct FlickerState* state, int damage) {
    if (damage == 1 && !state->isFlickering) {
        state->isFlickering = true;
        state->flickerTimer = 0.0f;
        state->elapsedFlickerTime = 0.0f;
        state->isVisible = true;
        printf("Flickering started\n");
    }
}

void update_flicker(struct FlickerState* state, float dt) {
    if (state->isFlickering) {
        state->flickerTimer += dt;
        state->elapsedFlickerTime += dt;

        // Toggle visibility at each flicker interval
        if (state->flickerTimer >= flickerInterval) {
            state->isVisible = !state->isVisible;
            state->flickerTimer = 0.0f; // Reset interval timer
        }

        // Stop flickering after the specified total duration
        if (state->elapsedFlickerTime >= flickerDuration) {
            state->isFlickering = false;
            state->isVisible = true; // Ensure visibility when flickering ends
            printf("Flickering ended\n");
        }
    }
}

bool is_entity_visible(const struct FlickerState* state) {
    return state->isVisible;
}

bool is_flickering(const struct FlickerState* state) {
    return state->isFlickering;
}
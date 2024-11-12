#pragma once
#include "cprocessing.h"
#include "utils.h"
#include <stdbool.h>

struct FlickerState {
    bool isFlickering;
    float flickerTimer;
    float elapsedFlickerTime;
    bool isVisible;
};

// Function prototypes
void flicker(struct FlickerState* state, int damage);
void update_flicker(struct FlickerState* state, float dt);
bool is_entity_visible(const struct FlickerState* state);
bool is_flickering(const struct FlickerState* state);
/******************************************************************************
* @filename		flicker.h
* @author(s)	Dalton koh
* @DP emails	d.koh@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file calls the functions necessary for damage invulnerability
                flickering effect.
******************************************************************************/

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
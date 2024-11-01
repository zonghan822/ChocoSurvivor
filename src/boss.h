#pragma once
#include "cprocessing.h"
#include "utils.h"

struct Boss {
    CP_Vector pos;
    float width;
    float height;
    float speed;
    CP_Image img;
    int maxHealth;
    bool isFlipped; // Track if the boss should be facing left
};
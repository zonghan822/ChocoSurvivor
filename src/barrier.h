#pragma once
#include "cprocessing.h"
#include "utils.h"

#define MAX_BARRIERS 10
#define BARRIER_INITIAL_HEALTH 5

struct Barrier {
    CP_Vector position;
    float width;
    float height;
    int health;
    struct Rectangle hitbox;
};

// Enum for barrier states
enum BarrierState {
    BARRIER_IDLE,
    BARRIER_PREVIEW
};

// Functions for managing barriers
void takeDamage(int damage);
int get_barrier_count(void);
void increment_barrier_count(void);
void set_barrier_properties(int index, CP_Vector position, float width, float height, int health);
struct Barrier* get_barriers(void);  // Add this line
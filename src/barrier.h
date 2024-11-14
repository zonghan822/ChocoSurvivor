#pragma once
#define MAX_BARRIERS 10
#include "cprocessing.h"
#define BARRIER_INITIAL_HEALTH 5

struct Barrier {
    CP_Vector position;
    float width;
    float height;
    int health;
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

struct allies {
    CP_Vector position;
    float width;
    float height;
    int health;
};

enum allieState {
    BARRIER_IDLE,
    BARRIER_PREVIEW
};
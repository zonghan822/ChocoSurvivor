#pragma once
#define MAX_BARRIERS 10

// Barrier struct to store position and size
struct Barrier {
    CP_Vector position;
    float width;
    float height;
    int health;
};

// Array to store placed barriers
struct Barrier barriers[MAX_BARRIERS];
int barrierCount = 0;  // Counter to keep track of the number of barriers placed

enum BarrierState {
    BARRIER_IDLE,
    BARRIER_PREVIEW
};
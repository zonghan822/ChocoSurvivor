#include "barrier.h"
#include <stdio.h>

static struct Barrier barriers[MAX_BARRIERS];  // Array to hold barriers
static int barrierCount = 0;                   // Counter for active barriers

// Function to return a pointer to the barriers array
struct Barrier* get_barriers(void) {
    return barriers;
}

void takeDamage(int damage) {
    for (int i = 0; i < barrierCount; i++) {
        if (barriers[i].health > 0) {
            barriers[i].health -= damage;
            if (barriers[i].health <= 0) {
                barriers[i].health = 0;  // Prevent negative health
                printf("Barrier %d has been destroyed.\n", i);
            }
            break;  // Exit after damaging the first active barrier
        }
    }
}

// Other functions for barrier management...
int get_barrier_count(void) {
    return barrierCount;
}

void increment_barrier_count(void) {
    if (barrierCount < MAX_BARRIERS) {
        barrierCount++;
    }
}

void set_barrier_properties(int index, CP_Vector position, float width, float height, int health) {
    if (index < MAX_BARRIERS) {
        barriers[index].position = position;
        barriers[index].width = width;
        barriers[index].height = height;
        barriers[index].health = health;
    }
}

#include "barrier.h"
#include <stdio.h>
#include "hittest.h"
#include "enemy.h"

static struct Barrier barriers[MAX_BARRIERS];  // Array to hold barriers
static int barrierCount = 0;                   // Counter for active barriers

// Function to return a pointer to the barriers array
struct Barrier* get_barriers(void) {
    return barriers;
}

void takeDamage(int damage) {

    int remainingDamage = damage; // Keep track of the damage to apply

    for (int i = 0; i < barrierCount && remainingDamage > 0; ) {
        if (barriers[i].health > 0) {
            barriers[i].health -= remainingDamage;

            if (barriers[i].health <= 0) {
                // Calculate leftover damage after the barrier is destroyed
                remainingDamage = -barriers[i].health;

                barriers[i].health = 0;  // Prevent negative health
                

                // Shift remaining barriers to fill the gap
                for (int j = i; j < barrierCount - 1; j++) {
                    barriers[j] = barriers[j + 1];
                }
                
                barrierCount--;  // Reduce barrier count

                // Do not increment `i`, as we need to check the new barrier at this index
            }
            else {
                // Barrier still has health, no remaining damage
                remainingDamage = 0;
                i++;  // Move to the next barrier
            }
        }
        else {
            i++;  // Skip barriers with no health
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
        barriers[index].health = 5;
        barriers[index].cooldownTimer = 0.0f; // Barrier starts with no cooldown
    }
}

void check_barrier_collision(struct ChocoRoach* roach, float dt) {
    for (int i = 0; i < barrierCount; i++) {
        struct Rectangle barrierRect = {
            .pos = barriers[i].position,
            .width = barriers[i].width,
            .height = barriers[i].height
        };

        // Decrease cooldown timer
        if (barriers[i].cooldownTimer > 0) {
            barriers[i].cooldownTimer -= dt;
        }

        // Check collision
        if (are_rects_intersecting(roach->hitbox, barrierRect)) {
            if (barriers[i].health > 0 && barriers[i].cooldownTimer <= 0) {
                barriers[i].health -= roach->dmg;
                barriers[i].cooldownTimer =  220.0f*dt ; // Set invincibility
                printf("Barrier %d takes damage! Health: %d\n", i, barriers[i].health);
            }

            if (barriers[i].health <= 0) {
                printf("Barrier %d has been destroyed by a ChocoRoach!\n", i);

                // Remove the barrier
                for (int j = i; j < barrierCount - 1; j++) {
                    barriers[j] = barriers[j + 1];
                }
                barrierCount--;
                i--; // Adjust index after removing a barrier
            }

            // Stop roach movement
            roach->speed = 0;
            return;
        }
    }
    // If no barrier collision, restore roach speed
    roach->speed = roach->originalSpeed;
}

void reset_barriers(void) {
    for (int i = 0; i < MAX_BARRIERS; i++) {
        barriers[i].health = 0;  // Mark barriers as inactive
        barriers[i].width = 0;   // Reset other properties if necessary
        barriers[i].height = 0;
    }
    barrierCount = 0;           // Reset the barrier count
    printf("All barriers have been reset.\n");
}

#include "barrier.h"
#include <stdio.h>
#include "hittest.h"
#include "enemy.h"
#include <float.h>

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


static struct Ally allies[MAX_ALLIES];  // Array to hold allies
static int allyCount = 0;               // Current count of active allies

// Initialize ally properties
void init_ally_properties(int index, CP_Vector position) {
    if (index < MAX_ALLIES) {
        allies[index].position = position;
        allies[index].width = 100.0f;
        allies[index].height = 150.0f;
        allies[index].health = 5;
        allies[index].maxHealth = 5;
        allies[index].attackSpeed = 1.0f;
        allies[index].attackPower = 20;
        allies[index].lastShotTime = 1.0f;
        allies[index].image = CP_Image_Load("Assets/ally_lvl1.png");

        // Initialize bullets
       // for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        //    allies[index].bullets[i].isShooting = FALSE;
        //}
    }
}



void update_allies(struct Ally* allies, int allyCount, struct ChocoRoach* enemies, int enemyCount, float dt) {
    for (int i = 0; i < allyCount; i++) {
        if (allies[i].health <= 0) continue;  // Skip dead allies

        // Increment the ally's shot timer
        allies[i].lastShotTime += dt;

        // Find the closest enemy
        struct ChocoRoach* closestEnemy = NULL;
        float closestDistance = FLT_MAX;
        for (int j = 0; j < enemyCount; j++) {
            if (enemies[j].currentState == STATE_DIED) continue;  // Skip dead enemies
            float distance = CP_Vector_Distance(allies[i].position, enemies[j].pos);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = &enemies[j];
            }
        }
        printf("Ally %d: lastShotTime = %.2f, attackSpeed = %.2f\n", i, allies[i].lastShotTime, allies[i].attackSpeed);
        // If an enemy is found and enough time has passed, shoot at the enemy
        if (closestEnemy != NULL && allies[i].lastShotTime >= allies[i].attackSpeed) {
            printf("Ally %d is shooting at enemy. Resetting lastShotTime.\n", i);
            struct Bullet bullet = init_bullet(
                NORMAL_BULLET,
                ALLY_BULLET_SPEED,
                10.0f,
                allies[i].attackPower,
                CP_Vector_Subtract(closestEnemy->pos, allies[i].position)
            );

            // Assign bullet to an available slot
            for (int k = 0; k < MAX_BULLET_COUNT; k++) {
                if (!allies[i].bullets[k].isShooting) {
                    bullet.pos = allies[i].position;  // Set bullet position to ally
                    allies[i].bullets[k] = bullet;
                    break;
                }
            }

            // Reset the cooldown timer
            allies[i].lastShotTime = 0.0f;
        }

        // Update ally bullets
        for (int k = 0; k < MAX_BULLET_COUNT; k++) {
            update_pos_bullet(&allies[i].bullets[k], dt);
        }
    }
}


// Render allies
void render_allies(struct Ally* allies, int allyCount, struct Camera cam) {
    for (int i = 0; i < allyCount; i++) {
        if (allies[i].health <= 0) continue;  // Skip dead allies

        // Render ally
        CP_Vector vec = get_cam_pos_rel(allies[i].position, cam);
        CP_Image_Draw(allies[i].image, vec.x, vec.y, allies[i].width, allies[i].height, 255);

        // Render bullets
        for (int k = 0; k < MAX_BULLET_COUNT; k++) {
            render_bullet(allies[i].bullets[k], cam);
        }
    }
}

// Shoot ally bullet
void shoot_ally_bullet(struct Ally* ally, struct Bullet* bullet, struct ChocoRoach* target) {
    int slot = -1;

    // Find the next available bullet slot
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        if (!ally->bullets[i].isShooting) {
            slot = i;
            break;
        }
    }

    // If there is an available slot, shoot the bullet
    if (slot != -1) {
        bullet->pos = ally->position;
        ally->bullets[slot] = *bullet;
    }
}

// Add a new ally
void add_ally(CP_Vector position) {
    if (allyCount < MAX_ALLIES) {
        init_ally_properties(allyCount, position);
        allyCount++;
        printf("Ally added! Total allies: %d\n", allyCount);
    }
    else {
        printf("Max allies reached.\n");
    }
}

struct Ally* get_allies(void) {
    return allies;
}

int get_ally_count(void) {
    return allyCount;
}
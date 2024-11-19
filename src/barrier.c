#include "barrier.h"
#include <stdio.h>
#include <float.h>
#include "hittest.h"
#include "enemy.h"
#include "level1.h"
#include "camera.h"

// Barrier variables
static struct Barrier barriers[MAX_BARRIERS];
static int barrierCount = 0;

// Ally variables
static struct Ally allies[MAX_ALLIES];
static int allyCount = 0;
static enum AllyState currentAllyState = ALLY_IDLE;
static CP_Vector allyPreviewPosition = { 0, 0 };

// Barrier Functions
struct Barrier* get_barriers(void) {
    return barriers;
}

int get_barrier_count(void) {
    return barrierCount;
}

void increment_barrier_count(void) {
    if (barrierCount < MAX_BARRIERS) {
        barrierCount++;
        printf("Barrier count incremented: %d\n", barrierCount);
    }
    else {
        printf("Max barriers reached. Cannot increment.\n");
    }
}

void set_barrier_properties(int index, CP_Vector position, float width, float height, int health) {
    if (index < MAX_BARRIERS) {
        barriers[index].position = position;
        barriers[index].width = width;
        barriers[index].height = height;
        barriers[index].health = health;
        barriers[index].cooldownTimer = 0.0f;
        printf("Barrier %d set at position (%.2f, %.2f) with health %d\n", index, position.x, position.y, health);
    }
}

void reset_barriers(void) {
    for (int i = 0; i < MAX_BARRIERS; i++) {
        barriers[i].health = 0;
        barriers[i].width = 0;
        barriers[i].height = 0;
    }
    barrierCount = 0;
    printf("All barriers reset.\n");
}

// Ally Functions
struct Ally* get_allies(void) {
    return allies;
}

int get_ally_count(void) {
    return allyCount;
}

void increment_ally_count(void) {
    if (allyCount < MAX_ALLIES) {
        allyCount++;
      //  printf("DEBUG: Incremented ally count to %d.\n", allyCount);
    }
    else {
       // printf("DEBUG: Max allies reached. Cannot increment.\n");
    }
}

void init_ally_properties(int index, CP_Vector position, float width, float height, int health) {
    if (index < MAX_ALLIES) {
        allies[index].position = position;
        allies[index].width = width;
        allies[index].height = height;
        allies[index].health = health;
        allies[index].maxHealth = health;
        allies[index].attackSpeed = 2.0f; // in milliseconds
        allies[index].attackPower = 20;
        allies[index].lastShotTime = 0.0f;

        allies[index].image = get_ally_image();

        // Initialize bullets
        for (int i = 0; i < MAX_BULLET_COUNT; i++) {
            allies[index].bullets[i].isShooting = FALSE;
        }


    }
    else {
       // printf("DEBUG: Unable to initialize ally at index %d (exceeds MAX_ALLIES).\n", index);
    }
}

static CP_Image allyImage = NULL;  // Static variable to store the ally image

CP_Image get_ally_image(void) {
    if (allyImage == NULL) {
        allyImage = CP_Image_Load("Assets/ally_lvl1.png");
    }
    return allyImage;
}

void render_allies(struct Camera cam) {
    struct Ally* allies = get_allies();
    for (int i = 0; i < get_ally_count(); i++) {
        if (allies[i].health > 0) {
            CP_Vector vec = get_cam_pos_rel(allies[i].position, cam);
            CP_Image_Draw(allies[i].image, vec.x, vec.y, allies[i].width, allies[i].height, 255);
        }
        else {
           // printf("DEBUG: Ally %d is inactive. Skipping render.\n", i);
        }
    }
}

void render_abullet(struct Bullet bullet, struct Camera cam) {
    if (bullet.isShooting) { // Only render bullets that are active
        CP_Vector screenPos = get_cam_pos_rel(bullet.pos, cam); // Convert world to screen coordinates
        CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));      // Red color for bullets
        CP_Graphics_DrawCircle(screenPos.x, screenPos.y, bullet.diameter); // Draw the bullet
        printf("DEBUG: Rendering bullet at position (%.2f, %.2f), diameter=%.2f\n",
            screenPos.x, screenPos.y, bullet.diameter);
    }
}
void update_allies(float dt, struct ChocoRoach* enemies, int enemyCount, struct Camera cam) {
    for (int i = 0; i < allyCount; i++) {
        if (allies[i].health <= 0) {
            printf("DEBUG: Ally %d is inactive. Skipping update.\n", i);
            continue;
        }

        // Update time since last shot
        allies[i].lastShotTime += dt;
        printf("DEBUG: Ally %d lastShotTime=%.2f, attackSpeed=%.2f\n", i, allies[i].lastShotTime, allies[i].attackSpeed);

        // Find the closest enemy
        struct ChocoRoach* closestEnemy = NULL;
        float closestDistance = 500.0f;

        for (int j = 0; j < enemyCount; j++) {
            if (enemies[j].currentState == STATE_DIED) continue;

            float distance = CP_Vector_Distance(allies[i].position, enemies[j].pos);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = &enemies[j];
            }
        }

        if (closestEnemy == NULL) {
            // No valid enemy detected; stop shooting and skip this ally's logic
            printf("DEBUG: No valid enemy detected for Ally %d. Stopping bullets.\n", i);
            for (int i = 0; i < allyCount; i++) { // Iterate over all allies
                for (int k = 0; k < MAX_BULLET_COUNT; k++) { // Iterate over all bullets for each ally
                    allies[i].bullets[k].isShooting = FALSE; // Reset isShooting flag
                }
            }
            continue; // Skip further logic for this ally
        }

        printf("DEBUG: Closest enemy to Ally %d: Position=(%.2f, %.2f), Distance=%.2f\n",
            i, closestEnemy->pos.x, closestEnemy->pos.y, closestDistance);

        // Shoot if ready and target found
        if (allies[i].lastShotTime >= allies[i].attackSpeed) {
            struct Bullet bullet = init_bullet(
                NORMAL_BULLET,
                ALLY_BULLET_SPEED,
                10.0f,
                allies[i].attackPower,
                CP_Vector_Subtract(closestEnemy->pos, allies[i].position)
            );

            // Find an available bullet slot
            int slot = find_bullet_slot(allies[i].bullets);
            if (slot != -1) {
                bullet.pos = allies[i].position;
                allies[i].bullets[slot] = bullet;
                allies[i].bullets[slot].isShooting = TRUE; // Mark bullet as active
                printf("DEBUG: Ally %d shot a bullet at enemy at position (%.2f, %.2f).\n", i, closestEnemy->pos.x, closestEnemy->pos.y);
            }
            else {
                printf("DEBUG: No available bullet slot for Ally %d.\n", i);
            }

            // Reset shooting cooldown
            allies[i].lastShotTime = 0.0f;
        }

        // Update all bullets for this ally
        for (int k = 0; k < MAX_BULLET_COUNT; k++) {
            if (allies[i].bullets[k].isShooting) {
                update_pos_bullet(&allies[i].bullets[k], dt);
                render_abullet(allies[i].bullets[k], cam); // Render the bullet
                printf("DEBUG: Bullet %d for Ally %d is moving. Position=(%.2f, %.2f)\n",
                    k, i, allies[i].bullets[k].pos.x, allies[i].bullets[k].pos.y);
            }
        }
    }
}





int find_bullet_slot(struct Bullet* bullets) {
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        if (!bullets[i].isShooting) {
            return i;
        }
    }
    return -1;
}


void reset_allies(void) {
    for (int i = 0; i < MAX_ALLIES; i++) {
        if (allies[i].health > 0) {
           // printf("DEBUG: Skipping active ally %d during reset (health: %d).\n", i, allies[i].health);
            continue; // Skip resetting active allies
        }
        allies[i].health = 0;
        allies[i].width = 0;
        allies[i].height = 0;
        allies[i].position = CP_Vector_Set(0, 0); // Reset position to default
        allies[i].lastShotTime = 0.0f;

        // Reset bullets for each ally
        for (int j = 0; j < MAX_BULLET_COUNT; j++) {
            allies[i].bullets[j].isShooting = FALSE;
        }

        //printf("DEBUG: Reset Ally %d to default values.\n", i);
    }
   // printf("DEBUG: All inactive allies reset. Total active allies: %d.\n", get_ally_count());
}
void check_barrier_collision(struct ChocoRoach* roach, float dt) {
    for (int i = 0; i < barrierCount; i++) {
        struct Rectangle barrierRect = {
            .pos = barriers[i].position,
            .width = barriers[i].width,
            .height = barriers[i].height
        };

        // Debugging: Display barrier properties
      //  printf("DEBUG: Checking collision for Barrier %d: position=(%.2f, %.2f), width=%.2f, height=%.2f, health=%d\n",
           // i, barriers[i].position.x, barriers[i].position.y, barriers[i].width, barriers[i].height, barriers[i].health);

        if (barriers[i].cooldownTimer > 0) {
            barriers[i].cooldownTimer -= dt;
       //     printf("DEBUG: Barrier %d is on cooldown. Timer: %.2f\n", i, barriers[i].cooldownTimer);
        }

        if (are_rects_intersecting(roach->hitbox, barrierRect)) {
        //    printf("DEBUG: Roach collided with Barrier %d.\n", i);

            if (barriers[i].health > 0 && barriers[i].cooldownTimer <= 0) {
                barriers[i].health -= roach->dmg;
                barriers[i].cooldownTimer = 500.0f;  // Cooldown time in milliseconds
          //      printf("DEBUG: Barrier %d took damage. New health: %d\n", i, barriers[i].health);
            }

            if (barriers[i].health <= 0) {
            //    printf("DEBUG: Barrier %d destroyed.\n", i);

                // Shift remaining barriers down to maintain array order
                for (int j = i; j < barrierCount - 1; j++) {
                    barriers[j] = barriers[j + 1];
                }
                barrierCount--;
                i--;  // Recheck the barrier at this index after shifting
            }

            roach->speed = 0;  // Stop the roach after collision
            return;  // Exit as roach can only hit one barrier at a time
        }
    }

    // If no collision, reset the roach's speed
    roach->speed = roach->originalSpeed;
  //  printf("DEBUG: No collision detected. Roach speed reset to %.2f.\n", roach->originalSpeed);
}
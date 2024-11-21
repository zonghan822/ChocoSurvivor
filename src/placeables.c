#include "placeables.h"
#include <stdio.h>
#include <float.h>
#include <stdlib.h>
#include "hittest.h"
#include "enemy.h"
#include "level1.h"
#include "camera.h"

// Barrier variables
struct Barrier barriers[MAX_BARRIERS];
static int barrierCount = 0;
CP_Image barrierImages[MAX_BARRIER_IMAGES] = { 0 };

// Ally variables
struct Ally allies[MAX_ALLIES];
static int allyCount = 0;
static enum AllyState currentAllyState = ALLY_IDLE;
static CP_Vector allyPreviewPosition = { 0, 0 };
CP_Image allyImages[MAX_ALLIES_IMAGES] = { 0 };


// Barrier Functions
void init_barrier_images(void) {
    barrierImages[0] = CP_Image_Load("Assets/barrier_1.png");
    barrierImages[1] = CP_Image_Load("Assets/barrier_2.png");
    barrierImages[2] = CP_Image_Load("Assets/barrier_3.png");
    barrierImages[3] = CP_Image_Load("Assets/barrier_4.png");
}

int get_barrier_count(void) {
    return barrierCount;
}

void increment_barrier_count(void) {
    if (barrierCount < MAX_BARRIERS) {
        barrierCount++;
    }
    else {
        printf("Max barriers reached. Cannot increment.\n");
    }
}

void set_barrier_properties(int index, CP_Vector position, float width, float height, int health) {
    if (index < MAX_BARRIERS) {
        barriers[index].position = barriers[index].hitbox.pos = position;
        barriers[index].width = barriers[index].hitbox.width = width;
        barriers[index].height = barriers[index].hitbox.height = height;
        barriers[index].health = barriers[index].maxHealth = health;
        barriers[index].isTakingDamage = false;
        barriers[index].healthBarDisplayTime = 0.f;
        printf("Barrier %d set at position (%.2f, %.2f) with health %d\n", index, position.x, position.y, health);
    }
}

CP_Image get_barrier_image(int barrierLevel) {
    return barrierImages[barrierLevel];
}

// Returns 1 if still alive, 0 if not
int barrier_take_damage(int barrierIndex, int damage) {
    barriers[barrierIndex].isTakingDamage = true;
    barriers[barrierIndex].health = CP_Math_ClampInt(barriers[barrierIndex].health - damage, 0, barriers[barrierIndex].maxHealth);

    if (barriers[barrierIndex].health > 0) {
        printf("Barrier took %d damage, health is now %d\n", damage, barriers[barrierIndex].health);
        return 1;
    }

    printf("Barrier destroyed!\n");

    if (barrierIndex != barrierCount - 1) {
        struct Barrier tmp = barriers[barrierIndex];
        barriers[barrierIndex] = barriers[barrierCount - 1];
        barriers[barrierCount - 1] = tmp;
    }
    barrierCount--;
    return 0;
}

void render_barriers(struct Camera cam) {
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255)); // Solid blue
    for (int i = 0; i < get_barrier_count(); i++) {
        if (barriers[i].health > 0) {  // Only draw if health is greater than zero
            CP_Vector vec = get_cam_pos_rel(barriers[i].position, cam);
            CP_Image_Draw(get_barrier_image(playerInstance.buffs.barrierLevel),
                vec.x, vec.y,
                barriers[i].width, barriers[i].height,
                255);
        }
    }
}

void dispose_barriers(void) {
    for (int i = 0; i < MAX_BARRIER_IMAGES; i++) {
        if (barrierImages[i] != NULL) {
            CP_Image_Free(&barrierImages[i]);
        }
    }
    barrierCount = 0;
    memset(barriers, 0, sizeof(barriers));
}


// Ally Functions
void init_ally_images(void) {
    allyImages[0] = CP_Image_Load("Assets/ally_lvl1.png");
    /*allyImages[1] = CP_Image_Load("Assets/barrier_2.png");
    allyImages[2] = CP_Image_Load("Assets/barrier_3.png");
    allyImages[3] = CP_Image_Load("Assets/barrier_4.png");*/
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

void init_ally(int index, CP_Vector position, float width, float height) {
    if (index < MAX_ALLIES) {
        allies[index].position = position;
        allies[index].width = width;
        allies[index].height = height;
        allies[index].attackSpeed = 2.0f; // in milliseconds
        allies[index].attackPower = 20;
        allies[index].lastShotTime = 0.0f;

        // Initialize bullets
        for (int i = 0; i < MAX_BULLET_COUNT; i++) {
            allies[index].bullets[i].isShooting = FALSE;
        }


    }
    else {
       // printf("DEBUG: Unable to initialize ally at index %d (exceeds MAX_ALLIES).\n", index);
    }
}

void render_allies(struct Camera cam) {
    for (int i = 0; i < get_ally_count(); i++) {
        if (!allies[i].state) {
            CP_Vector vec = get_cam_pos_rel(allies[i].position, cam);
            CP_Image_Draw(get_ally_image(playerInstance.buffs.allyLevel),
                vec.x, vec.y,
                allies[i].width, allies[i].height,
                255);
        }
    }
}

void render_ally_bullet(struct Bullet bullet, struct Camera cam) {
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
        if (allies[i].state < ALLY_IDLE)
            continue;

        // Update time since last shot
        allies[i].lastShotTime += dt;
        printf("DEBUG: Ally %d lastShotTime=%.2f, attackSpeed=%.2f\n", i, allies[i].lastShotTime, allies[i].attackSpeed);

        // Find the closest enemy
        struct ChocoRoach* closestEnemy = NULL;
        float closestDistance = 500.0f;

        for (int j = 0; j < enemyCount; j++) {
            if (enemies[j].currentState == ENEMY_STATE_DIED) continue;

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
                render_ally_bullet(allies[i].bullets[k], cam); // Render the bullet
                printf("DEBUG: Bullet %d for Ally %d is moving. Position=(%.2f, %.2f)\n",
                    k, i, allies[i].bullets[k].pos.x, allies[i].bullets[k].pos.y);
            }
        }
    }
}

CP_Image get_ally_image(int allyLevel) {
    return allyImages[allyLevel];
}

int find_bullet_slot(struct Bullet* bullets) {
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        if (!bullets[i].isShooting) {
            return i;
        }
    }
    return -1;
}

void dispose_allies(void) {
    for (int i = 0; i < MAX_ALLIES_IMAGES; i++) {
        if (allyImages[i] != NULL) {
            CP_Image_Free(&allyImages[i]);
        }
    }
    allyCount = 0;
    memset(allies, 0, sizeof(allies));
}
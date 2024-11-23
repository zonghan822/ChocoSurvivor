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
int barrierCount = 0;
CP_Image barrierImages[MAX_BARRIER_IMAGES] = { 0 };

// Ally variables
struct Ally allies[MAX_ALLIES];
int allyCount = 0;
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

void set_barrier_properties(int index, CP_Vector pos, float width, float height, int health) {
    if (index < MAX_BARRIERS) {
        barriers[index].pos = barriers[index].hitbox.pos = pos;
        barriers[index].width = barriers[index].hitbox.width = width;
        barriers[index].height = barriers[index].hitbox.height = height;
        barriers[index].health = barriers[index].maxHealth = health;
        barriers[index].isTakingDamage = false;
        barriers[index].healthBarDisplayTime = 0.f;
        printf("Barrier %d set at position (%.2f, %.2f) with health %d\n", index, pos.x, pos.y, health);
    }
}

CP_Image get_barrier_image(int barrierLevel) {
    return barrierImages[barrierLevel];
}

static int _comp(const void* a, const void* b)
{
    if (!(((struct Barrier*)a)->health) && !(((struct Barrier*)b)->health))
        return -1;
    if (!(((struct Barrier*)a)->health))
        return 1;
    if (!(((struct Barrier*)b)->health))
        return -1;
    return ((struct Barrier*)a)->pos.y - ((struct Barrier*)b)->pos.y;
}

void sort_barriers(void) {
    qsort(barriers, MAX_BARRIERS, sizeof(struct Barrier), _comp);
}

// Returns 1 if still alive, 0 if not
int barrier_take_damage(int barrierIndex, int damage) {
    if (barriers[barrierIndex].health <= 0) // prevent deal damage to destroyed barrier
        return -1;

    barriers[barrierIndex].isTakingDamage = true;
    barriers[barrierIndex].health = CP_Math_ClampInt(barriers[barrierIndex].health - damage, 0, barriers[barrierIndex].maxHealth);
    barriers[barrierIndex].healthBarDisplayTime = 0;

    if (barriers[barrierIndex].health > 0) {
        printf("Barrier took %d damage, health is now %d\n", damage, barriers[barrierIndex].health);
        return 1;
    }

    printf("Barrier destroyed!\n");
    --barrierCount;
    return 0;
}

void render_barriers() {
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255)); // Solid blue
    for (int i = 0; i < get_barrier_count(); i++) {
        if (barriers[i].health > 0) {  // Only draw if health is greater than zero
            CP_Image_Draw(get_barrier_image(playerInstance.buffs.barrierLevel),
                barriers[i].pos.x, barriers[i].pos.y,
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

void init_ally(int index, CP_Vector pos, float width, float height) {
    if (index < MAX_ALLIES) {
        allies[index].pos = pos;
        allies[index].width = width;
        allies[index].height = height;
        allies[index].attackSpeed = 2.0f; // in milliseconds
        allies[index].attackPower = 20;
    }
    else {
       // printf("DEBUG: Unable to initialize ally at index %d (exceeds MAX_ALLIES).\n", index);
    }
}

void render_allies() {
    for (int i = 0; i < get_ally_count(); i++) {
        if (!allies[i].state) {
            // PLACEHOLDER
            // TODO: Remove
            int allyLevel = playerInstance.buffs.allyLevel; // PH: Stored here until allies have upgrade images
            CP_Image img = get_ally_image(0);
            float preservedHeight = get_preserved_heightf(CP_Image_GetWidth(img),
                CP_Image_GetHeight(img), allies[i].width);
            CP_Image_Draw(img,
                allies[i].pos.x, allies[i].pos.y,
                allies[i].width, preservedHeight,
                255);
        }
    }
}

CP_Image get_ally_image(int allyLevel) {
    return allyImages[allyLevel];
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
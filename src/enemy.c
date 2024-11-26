/******************************************************************************
*@filename		enemy.c
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This file is a header file for astar pathfinding algorithm for enemy and boss
movement.
******************************************************************************/

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"
#include "placeables.h"


struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float spawnWidth, float speed, int maxHealth, int dmg, float frameTime) {
    struct ChocoRoach chocoRoach = {0};

    // Set up enemy properties
    chocoRoach.roachFrames[0] = CP_Image_Load("Assets/walking animation/frame1.png");
    chocoRoach.roachFrames[1] = CP_Image_Load("Assets/walking animation/frame2.png");
    chocoRoach.roachFrames[2] = CP_Image_Load("Assets/walking animation/frame3.png");
    chocoRoach.roachFrames[3] = CP_Image_Load("Assets/walking animation/frame4.png");
    chocoRoach.roachFrames[4] = CP_Image_Load("Assets/walking animation/frame5.png");
    chocoRoach.roachFrames[5] = CP_Image_Load("Assets/walking animation/frame6.png");
    chocoRoach.width = ENEMY_WIDTH;
    chocoRoach.height = get_preserved_heightf(
        (float)CP_Image_GetWidth(chocoRoach.roachFrames[0]),
        (float)CP_Image_GetHeight(chocoRoach.roachFrames[0]),
        chocoRoach.width
    );
    chocoRoach.pos.x = CP_Random_RangeFloat(-chocoRoach.width - spawnWidth, -chocoRoach.width);
    chocoRoach.pos.y = CP_Random_RangeFloat(spawnMinY, spawnMaxY);
    chocoRoach.speed = speed;
    chocoRoach.health = maxHealth;
    chocoRoach.maxHealth = maxHealth;
    chocoRoach.dmg = dmg;
    chocoRoach.attackCooldown = 0.f;
    chocoRoach.moveDirection = CP_Vector_Zero();
    chocoRoach.currentState = ENEMY_STATE_IDLE;
    chocoRoach.hitbox.pos.x = chocoRoach.pos.x + ENEMY_HITBOX_OFFSET_X(chocoRoach.width);
    chocoRoach.hitbox.pos.y = chocoRoach.pos.y + ENEMY_HITBOX_OFFSET_Y(chocoRoach.height);
    chocoRoach.hitbox.width = 0.8f * chocoRoach.width;
    chocoRoach.hitbox.height = 0.75f * chocoRoach.height;
    chocoRoach.healthBarDisplayTime = 0.f;
    chocoRoach.isTakingDamage = false;
    chocoRoach.barrierAttack = MAX_BARRIERS;

    // Initialize animation properties
    chocoRoach.currentFrame = 0;
    chocoRoach.frameTime = frameTime; // Use the provided frame time
    chocoRoach.timeAccumulator = 0.0f;

    return chocoRoach;
}


void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
    int pathArray[100];
    int pathNodeCount = 0;
    int startNodeIndex = get_nearest_node_index(grid, nodeCount, roach->pos);
    int endNodeIndex = get_nearest_node_index(grid, nodeCount, hitbox.pos);
    if (startNodeIndex == endNodeIndex) {
        roach->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(hitbox.pos, roach->pos)));
    } else {
        pathNodeCount = find_astar_path(grid, nodeCount, startNodeIndex, endNodeIndex, pathArray);
        
        if (pathNodeCount) {
            roach->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(grid[pathArray[0]].pos, roach->pos)));
            // printf("Move Direction (%lf, %lf)\n", roach->moveDirection.x, roach->moveDirection.y);
        }
        else {
            int startNodeIndexNB = get_nearest_node_index(gridNoBarrier, nodeCount, roach->pos);
            int endNodeIndexNB = get_nearest_node_index(gridNoBarrier, nodeCount, hitbox.pos);
            find_astar_path(gridNoBarrier, nodeCount, startNodeIndexNB, endNodeIndexNB, pathArray);
            roach->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(grid[pathArray[0]].pos, roach->pos)));
        }
    }
    roach->width = (roach->moveDirection.x < 0.f && roach->width > 0.f) || (roach->moveDirection.x > 0.f && roach->width < 0.f) ? -roach->width: roach->width;
    roach->pos.x += roach->moveDirection.x * roach->speed * dt;
    roach->pos.y += roach->moveDirection.y * roach->speed * dt;
}

void render_enemies(struct ChocoRoach* roaches, int size, float deltaTime) {
    for (int i = 0; i < size; i++) {
        if (roaches[i].currentState != ENEMY_STATE_DIED) {
            // Update animation frame
            roaches[i].timeAccumulator += deltaTime;
            if (roaches[i].timeAccumulator >= roaches[i].frameTime) {
                roaches[i].timeAccumulator -= roaches[i].frameTime;
                roaches[i].currentFrame = (roaches[i].currentFrame + 1) % ROACH_FRAMES; // Cycle through frames
            }

            // Check for valid frame and dimensions
            if (!roaches[i].roachFrames[roaches[i].currentFrame]) {
                printf("Error: Enemy %d frame %d is NULL. Skipping draw.\n", i, roaches[i].currentFrame);
                continue;
            }
            if (roaches[i].width <= 0 || roaches[i].height <= 0) {
                printf("Error: Invalid dimensions for Enemy %d. Skipping draw.\n", i);
                continue;
            }

            // Draw current frame
            CP_Image_Draw(roaches[i].roachFrames[roaches[i].currentFrame],
                roaches[i].pos.x, roaches[i].pos.y,
                roaches[i].width, roaches[i].height, 255);
        }
    }
}

static int _comp(const void* a, const void* b)
{
    struct ChocoRoach* roachA = (struct ChocoRoach*)a;
    struct ChocoRoach* roachB = (struct ChocoRoach*)b;

    if (!roachA->currentState && !roachB->currentState)
        return -1;
    if (!roachA->currentState)
        return 1;
    if (!roachB->currentState)
        return -1;
    // if (roachA->pos.y == roachB->pos.y)
    //     return roachA->pos.x - roachB->pos.x;

    return (int)(roachA->pos.y - roachB->pos.y);
}

void sort_enemies(struct ChocoRoach* roaches, int size) {
    qsort(roaches, size, sizeof(struct ChocoRoach), _comp);
}

// return 1 if enemy is killed, 0 otherwise
int enemy_take_damage(struct ChocoRoach* roach, int dmg) {
    roach->health = CP_Math_ClampInt(roach->health - dmg, 0, roach->maxHealth);
    roach->isTakingDamage = true;
    if (roach->health <= 0) {
        roach->currentState = ENEMY_STATE_DIED;
        return 1;
    }
    return 0;
}

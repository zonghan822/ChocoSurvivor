#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"


struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float spawnWidth, float speed, int maxHealth, int dmg) {
    srand((unsigned int)time(NULL));
    struct ChocoRoach chocoRoach;
    chocoRoach.img = CP_Image_Load("Assets/choco_roach.png");
    chocoRoach.width = ENEMY_WIDTH;
    chocoRoach.height = get_preserved_heightf((float)CP_Image_GetWidth(chocoRoach.img), (float)CP_Image_GetHeight(chocoRoach.img), chocoRoach.width);
    chocoRoach.pos.x = CP_Random_RangeFloat(-chocoRoach.width - spawnWidth, -chocoRoach.width);
    chocoRoach.pos.y = CP_Random_RangeFloat(spawnMinY, spawnMaxY); // Generate random y pos for enemies
    chocoRoach.speed = speed;
    chocoRoach.health = maxHealth;
    chocoRoach.maxHealth = maxHealth;
    chocoRoach.dmg = dmg;
    chocoRoach.attackCooldown = 0.f;
    chocoRoach.moveDirection = CP_Vector_Zero();
    chocoRoach.currentState = ENEMY_STATE_IDLE;
    chocoRoach.hitbox.pos.x = chocoRoach.pos.x + ENEMY_HITBOX_OFFSET_X(chocoRoach.width);
    chocoRoach.hitbox.pos.y = chocoRoach.pos.y + ENEMY_HITBOX_OFFSET_Y(chocoRoach.height);
    chocoRoach.hitbox.width = 0.8 * chocoRoach.width;
    chocoRoach.hitbox.height = 0.75 * chocoRoach.height;
    chocoRoach.healthBarDisplayTime = 0.f;
    chocoRoach.isTakingDamage = false;
    chocoRoach.barrierAttack = -1;
    return chocoRoach;
}

void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
    int pathArray[50];
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

void render_enemies(struct ChocoRoach* roaches, struct Camera cam, int size) {
    for (int i = 0; i < size; i++) {
        if (roaches[i].currentState != ENEMY_STATE_DIED) {
            CP_Vector vec = get_cam_pos_rel(roaches[i].pos, cam);
            CP_Image_Draw(roaches[i].img,
                vec.x, vec.y,
                roaches[i].width, roaches[i].height,
                255);
            //printf("enemy %d pos: ( %lf, %lf )\n", i, roaches[i].pos.x, roaches[i].pos.y);
        }   
    }
}

static int _comp(const void* a, const void* b)
{
    if (!(((struct ChocoRoach*)a)->currentState) && !(((struct ChocoRoach*)b)->currentState))
        return -1;
    if (!(((struct ChocoRoach*)a)->currentState))
        return 1;
    if (!(((struct ChocoRoach*)b)->currentState))
        return -1;
    /*if (((struct ChocoRoach*)a)->pos.y == ((struct ChocoRoach*)b)->pos.y)
        return ((struct ChocoRoach*)a)->pos.x - ((struct ChocoRoach*)b)->pos.x;*/
    return ((struct ChocoRoach*)a)->pos.y - ((struct ChocoRoach*)b)->pos.y;
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
        // set element to 0
        memset(roach, 0, sizeof(roach));
        return 1;
    }
    return 0;
}

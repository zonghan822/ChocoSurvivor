#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"

void empty_enemies(struct ChocoRoach* roaches) {
    for (int i = 0; i < MAX_ENEMY_COUNT; i++) {
        roaches->currentState = 0;
    }
}

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
    chocoRoach.moveDirection = CP_Vector_Zero();
    chocoRoach.currentState = STATE_IDLE;
    chocoRoach.hitbox.pos.x = chocoRoach.pos.x + ENEMY_HITBOX_OFFSET_X(chocoRoach.width);
    chocoRoach.hitbox.pos.y = chocoRoach.pos.y + ENEMY_HITBOX_OFFSET_Y(chocoRoach.height);
    chocoRoach.hitbox.width = 0.8 * chocoRoach.width;
    chocoRoach.hitbox.height = 0.75 * chocoRoach.height;
    return chocoRoach;
}

void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt, PathNode* grid, int nodeCount) {
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
        }
        else {
            roach->moveDirection = CP_Vector_Zero();
            printf("No path!\n");
        }
    }
    roach->pos.x += roach->moveDirection.x * roach->speed * dt;
    roach->pos.y += roach->moveDirection.y * roach->speed * dt;
}

void render_enemy_health_bar(struct ChocoRoach* roach, struct Camera cam) {
    // get relative position after camera transformation
    CP_Vector vec = get_cam_pos_rel(roach->pos, cam);

    // Set position of health bar
    float health_bar_x = vec.x;
    float health_bar_y = (vec.y) - (roach->height / 2) - 10.f;
    float health_bar_w = roach->width * 0.8;
    float health_bar_h = 15.f;

    CP_Settings_RectMode(CP_POSITION_CENTER);

    // Draw health bar rect
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Graphics_DrawRect(health_bar_x, health_bar_y, health_bar_w, health_bar_h);

    // Compute position and width for health bar gap rect
    float health_gap_x = health_bar_x + ((float)(roach->health) / (float)(roach->maxHealth)) * health_bar_w;
    float health_gap_w = health_bar_w * (1.f - ((float)(roach->health) / (float)(roach->maxHealth)));

    // Draw health bar gap
    CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
    CP_Graphics_DrawRect(health_gap_x, health_bar_y, health_gap_w, health_bar_h);
}

void render_enemies(struct ChocoRoach* roaches, struct Camera cam, int size) {
    for (int i = 0; i < size; i++) {
        if (roaches[i].currentState != STATE_DIED) {
            CP_Vector vec = get_cam_pos_rel(roaches[i].pos, cam);
            CP_Image_Draw(
                roaches[i].img,
                vec.x,
                vec.y,
                roaches[i].width,
                roaches[i].height,
                255
            );
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
    if (((struct ChocoRoach*)a)->pos.y == ((struct ChocoRoach*)b)->pos.y)
        return ((struct ChocoRoach*)a)->pos.x - ((struct ChocoRoach*)b)->pos.x;
    return ((struct ChocoRoach*)a)->pos.y - ((struct ChocoRoach*)b)->pos.y;
}

void sort_enemies(struct ChocoRoach* roaches, int size) {
    qsort(roaches, size, sizeof(struct ChocoRoach), _comp);
}

// return 1 if enemy is killed, 0 otherwise
int enemy_take_damage(struct ChocoRoach* roach, int dmg) {
    roach->health = CP_Math_ClampInt(roach->health - dmg, 0, roach->maxHealth);
    roach->isTakingDamage = TRUE;
    if (roach->health <= 0) {
        roach->currentState = STATE_DIED;
        return 1;
    }
    return 0;
}
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"

struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float spawnWidth, float speed, int maxHealth, int dmg) {

    srand((unsigned int)time(NULL));
    struct ChocoRoach chocoRoach = {
            .img = CP_Image_Load("Assets/choco_roach.png"),
            .width = 70.f,
            .height = get_preserved_heightf((float)CP_Image_GetWidth(chocoRoach.img),
                (float)CP_Image_GetHeight(chocoRoach.img), chocoRoach.width),
            .pos.x = CP_Random_RangeFloat(-chocoRoach.width-spawnWidth, -chocoRoach.width),
            .pos.y = CP_Random_RangeFloat(spawnMinY, spawnMaxY), // Generate random y pos for enemies
            .speed = speed,
            .health = maxHealth,
            .maxHealth = maxHealth,
            .dmg = dmg,
            .moveDirection = CP_Vector_Zero(),
            .currentState = STATE_IDLE,
            .hitbox.pos.x = chocoRoach.pos.x + ENEMY_HITBOX_OFFSET_X(chocoRoach.width),
            .hitbox.pos.y = chocoRoach.pos.y + ENEMY_HITBOX_OFFSET_Y(chocoRoach.height),
            .hitbox.width = 0.8 * chocoRoach.width,
            .hitbox.height = 0.75 * chocoRoach.height,
    };
    return chocoRoach;
}

//void update_pos_enemy(struct ChocoRoach *roach, float dt) {
//    switch (roach->movement) {
//    case MOVING_UP:
//        roach->pos.y -= roach->speed * dt;
//        break;
//    case MOVING_RIGHT:
//        roach->pos.x += roach->speed * dt;
//        break;
//    case MOVING_DOWN:
//        roach->pos.y += roach->speed * dt;
//        break;
//    case MOVING_LEFT:
//        roach->pos.x -= roach->speed * dt;
//        break;
//    case MOVING_UP | MOVING_RIGHT:
//        roach->pos.x += get_diagonal_dist(roach->speed * dt);
//        roach->pos.y -= get_diagonal_dist(roach->speed * dt);
//        break;
//    case MOVING_UP | MOVING_LEFT:
//        roach->pos.x -= get_diagonal_dist(roach->speed * dt);
//        roach->pos.y -= get_diagonal_dist(roach->speed * dt);
//        break;
//    case MOVING_DOWN | MOVING_RIGHT:
//        roach->pos.x += get_diagonal_dist(roach->speed * dt);
//        roach->pos.y += get_diagonal_dist(roach->speed * dt);
//        break;
//    case MOVING_DOWN | MOVING_LEFT:
//        roach->pos.x -= get_diagonal_dist(roach->speed * dt);
//        roach->pos.y += get_diagonal_dist(roach->speed * dt);
//    }
//}

//void enemy_chase_player(struct ChocoRoach* roach, struct ChocoShooter player) {
//    roach->movement = STOPPED;
//    // chase player 
//    float enemyMinX = roach->hitbox.pos.x - (roach->hitbox.width / 2),
//        enemyMaxX = roach->hitbox.pos.x + (roach->hitbox.width / 2),
//        enemyMinY = roach->hitbox.pos.y - (roach->hitbox.height / 2),
//        enemyMaxY = roach->hitbox.pos.y + (roach->hitbox.height / 2),
//        playerMinX = player.hitbox.pos.x - (player.hitbox.width / 2),
//        playerMaxX = player.hitbox.pos.x + (player.hitbox.width / 2),
//        playerMinY = player.hitbox.pos.y - (player.hitbox.height / 2),
//        playerMaxY = player.hitbox.pos.y + (player.hitbox.height / 2);
//
//    if (enemyMinY > playerMaxY) // Player is above enemy
//        roach->movement |= MOVING_UP;
//    if (enemyMaxY < playerMinY) // Player is below enemy
//        roach->movement |= MOVING_DOWN;
//    if (enemyMinX > playerMaxX) // Player is at the left of enemy 
//        roach->movement |= MOVING_LEFT;
//    if (enemyMaxX < playerMinX) // Player is at the right of enemy 
//        roach->movement |= MOVING_RIGHT;
//}
//
//void enemy_move_to_tower(struct ChocoRoach* roach, struct DefenseTarget tower) {
//    roach->movement = STOPPED;
//    float enemyMinX = roach->hitbox.pos.x - (roach->hitbox.width / 2),
//        enemyMaxX = roach->hitbox.pos.x + (roach->hitbox.width / 2),
//        enemyMinY = roach->hitbox.pos.y - (roach->hitbox.height / 2),
//        enemyMaxY = roach->hitbox.pos.y + (roach->hitbox.height / 2),
//        towerMinX = tower.pos.x - (tower.width / 2),
//        towerMaxX = tower.pos.x + (tower.width / 2),
//        towerMinY = tower.pos.y - (tower.height / 2),
//        towerMaxY = tower.pos.y + (tower.height / 2);
//
//    if (enemyMinY > towerMaxY) // Tower is above enemy
//        roach->movement |= MOVING_UP;
//    if (enemyMaxY < towerMinY) // Tower is below enemy
//        roach->movement |= MOVING_DOWN;
//    if (enemyMinX > towerMaxX) // Tower is at the left of enemy 
//        roach->movement |= MOVING_LEFT;
//    if (enemyMaxX < towerMinX) // Tower is at the right of enemy 
//        roach->movement |= MOVING_RIGHT;
//}

void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt) {
    roach->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(hitbox.pos, roach->pos)));
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

int _comp(const void* a, const void* b)
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

void enemy_take_damage(struct ChocoRoach* roach, int dmg) {
    roach->health = CP_Math_ClampInt(roach->health - dmg, 0, roach->maxHealth);
    roach->isTakingDamage = TRUE;
    if (roach->health <= 0) {
        roach->currentState = STATE_DIED;
    }
}
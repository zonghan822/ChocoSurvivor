#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "cprocessing.h"
#include "enemy.h"
#include "camera.h"

struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float speed, int maxHealth, int dmg) {

    srand((unsigned int)time(NULL));
    struct ChocoRoach chocoRoach = {
            .img = CP_Image_Load("Assets/choco_roach.png"),
            .width = 70.f,
            .height = get_preserved_heightf((float)CP_Image_GetWidth(chocoRoach.img),
                (float)CP_Image_GetHeight(chocoRoach.img), chocoRoach.width),
            .pos.x = -chocoRoach.width,
            .pos.y = CP_Random_RangeFloat(spawnMinY, spawnMaxY), // Generate random y pos for enemies
            .speed = speed,
            .health = maxHealth,
            .maxHealth = maxHealth,
            .dmg = dmg,
            .movement = MOVING_RIGHT,
            .currentState = STATE_MOVING,
            .hitbox.pos.x = chocoRoach.pos.x + ENEMY_HITBOX_OFFSET_X(chocoRoach.width),
            .hitbox.pos.y = chocoRoach.pos.y + ENEMY_HITBOX_OFFSET_Y(chocoRoach.height),
            .hitbox.width = 0.8 * chocoRoach.width,
            .hitbox.height = 0.75 * chocoRoach.height
    };
    return chocoRoach;
}

void render_enemy(struct ChocoRoach roach, struct Camera cam) {
    CP_Vector vec = get_cam_pos_rel(roach.pos, cam);
	CP_Image_Draw(
		roach.img,
		vec.x,
		vec.y,
        roach.width,
        roach.height,
		255
	);
}

void update_pos_enemy(struct ChocoRoach *roach, float dt) {
    switch (roach->movement) {
    case MOVING_UP:
        roach->pos.y -= roach->speed * dt;
        break;
    case MOVING_RIGHT:
        roach->pos.x += roach->speed * dt;
        break;
    case MOVING_DOWN:
        roach->pos.y += roach->speed * dt;
        break;
    case MOVING_LEFT:
        roach->pos.x -= roach->speed * dt;
        break;
    case MOVING_UP | MOVING_RIGHT:
        roach->pos.x += get_diagonal_dist(roach->speed * dt);
        roach->pos.y -= get_diagonal_dist(roach->speed * dt);
        break;
    case MOVING_UP | MOVING_LEFT:
        roach->pos.x -= get_diagonal_dist(roach->speed * dt);
        roach->pos.y -= get_diagonal_dist(roach->speed * dt);
        break;
    case MOVING_DOWN | MOVING_RIGHT:
        roach->pos.x += get_diagonal_dist(roach->speed * dt);
        roach->pos.y += get_diagonal_dist(roach->speed * dt);
        break;
    case MOVING_DOWN | MOVING_LEFT:
        roach->pos.x -= get_diagonal_dist(roach->speed * dt);
        roach->pos.y += get_diagonal_dist(roach->speed * dt);
    }
}


void chase_player(struct ChocoRoach* roach, struct ChocoShooter player) {
    roach->movement = STOPPED;
    // chase player 
    float enemyMinX = roach->hitbox.pos.x - (roach->hitbox.width / 2),
        enemyMaxX = roach->hitbox.pos.x + (roach->hitbox.width / 2),
        enemyMinY = roach->hitbox.pos.y - (roach->hitbox.height / 2),
        enemyMaxY = roach->hitbox.pos.y + (roach->hitbox.height / 2),
        playerMinX = player.hitbox.pos.x - (player.hitbox.width / 2),
        playerMaxX = player.hitbox.pos.x + (player.hitbox.width / 2),
        playerMinY = player.hitbox.pos.y - (player.hitbox.height / 2),
        playerMaxY = player.hitbox.pos.y + (player.hitbox.height / 2);

    if (enemyMinY > playerMaxY) // Player is above enemy
        roach->movement |= MOVING_UP;
    if (enemyMaxY < playerMinY) // Player is below enemy
        roach->movement |= MOVING_DOWN;
    if (enemyMinX > playerMaxX) // Player is at the left of enemy 
        roach->movement |= MOVING_LEFT;
    if (enemyMaxX < playerMinX) // Player is at the right of enemy 
        roach->movement |= MOVING_RIGHT;
}
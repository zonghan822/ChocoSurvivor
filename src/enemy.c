#include <math.h>
#include "cprocessing.h"
#include "enemy.h"

void render_enemy(struct ChocoRoach roach) {
	CP_Image_Draw(
		roach.img,
		roach.pos.x,
		roach.pos.y,
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
    float enemyMinX = roach->pos.x - (roach->width / 2),
        enemyMaxX = roach->pos.x + (roach->width / 2),
        enemyMinY = roach->pos.y - (roach->height / 2),
        enemyMaxY = roach->pos.y + (roach->height / 2),
        playerMinX = player.pos.x - (player.width / 2),
        playerMaxX = player.pos.x + (player.width / 2),
        playerMinY = player.pos.y - (player.height / 2),
        playerMaxY = player.pos.y + (player.height / 2);

    if (enemyMinY > playerMaxY) // Player is above enemy
        roach->movement |= MOVING_UP;
    if (enemyMaxY < playerMinY) // Player is below enemy
        roach->movement |= MOVING_DOWN;
    if (enemyMinX > playerMaxX) // Player is at the left of enemy 
        roach->movement |= MOVING_LEFT;
    if (enemyMaxX < playerMinX) // Player is at the right of enemy 
        roach->movement |= MOVING_RIGHT;
}
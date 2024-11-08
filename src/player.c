#include "cprocessing.h"
#include "player.h"
#include <stdio.h>

void takeDamage(struct ChocoShooter* player, int damage) {
    if (player->health > 0) {
        player->health -= damage;
        if (player->health < 0) {
            player->health = 0;  // Ensure health doesn't go below zero
            printf("Choco Shooter has been defeated!\n");
            // You can add code here to handle game-over or respawn logic
        }
        else {
            printf("Choco Shooter took %d damage, health is now %d\n", damage, player->health);
        }
    }
}

void move_player(struct ChocoShooter* player) {
    // Player movement and setting movement
    player->movement = STOPPED;
    if (CP_Input_KeyDown(KEY_W))
        player->movement |= MOVING_UP;
    if (CP_Input_KeyDown(KEY_S))
        player->movement |= MOVING_DOWN;
    if (CP_Input_KeyDown(KEY_A))
        player->movement |= MOVING_LEFT;
    if (CP_Input_KeyDown(KEY_D))
        player->movement |= MOVING_RIGHT;

    // W+S and A+D inputs should stop the character
    // Clear both UP and DOWN bits
    // Clear both LEFT and RIGHT bits
    if ((player->movement & MOVING_UP) && (player->movement & MOVING_DOWN))
        player->movement &= ~(MOVING_UP | MOVING_DOWN);
    if ((player->movement & MOVING_LEFT) && (player->movement & MOVING_RIGHT))
        player->movement &= ~(MOVING_LEFT | MOVING_RIGHT);
}

void update_pos_player(struct ChocoShooter* player, float dt) {
    switch (player->movement) {
    case MOVING_UP:
        player->pos.y -= player->speed * dt;
        break;
    case MOVING_RIGHT:
        player->pos.x += player->speed * dt;
        break;
    case MOVING_DOWN:
        player->pos.y += player->speed * dt;
        break;
    case MOVING_LEFT:
        player->pos.x -= player->speed * dt;
        break;
    case MOVING_UP | MOVING_RIGHT:
        player->pos.x += get_diagonal_dist(player->speed * dt);
        player->pos.y -= get_diagonal_dist(player->speed * dt);
        break;
    case MOVING_UP | MOVING_LEFT:
        player->pos.x -= get_diagonal_dist(player->speed * dt);
        player->pos.y -= get_diagonal_dist(player->speed * dt);
        break;
    case MOVING_DOWN | MOVING_RIGHT:
        player->pos.x += get_diagonal_dist(player->speed * dt);
        player->pos.y += get_diagonal_dist(player->speed * dt);
        break;
    case MOVING_DOWN | MOVING_LEFT:
        player->pos.x -= get_diagonal_dist(player->speed * dt);
        player->pos.y += get_diagonal_dist(player->speed * dt);
    }
}
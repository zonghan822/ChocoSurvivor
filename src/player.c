#include "cprocessing.h"
#include "player.h"
#include <stdio.h>
#include "flicker.h"
#include "game.h"


void takeDamage(struct ChocoShooter* player, int damage) {
    if (is_flickering(&playerFlickerState)) {
        printf("chocoshooter is invincible and cannot take damage right now.\n");
        return;  // Skip the rest of the function if flickering
    }

    if (player->health > 0) {
        player->health -= damage;
        if (player->health <= 0) {
            player->health = 0;  // Ensure health doesn't go below zero
            printf("Choco Shooter has been defeated!\n");
            player->img = 0;

        }
        else {
            printf("Choco Shooter took %d damage, health is now %d\n", damage, player->health);
            if (!is_flickering(&playerFlickerState)) {
                flicker(&playerFlickerState, 1);
            }
        }
    }
}
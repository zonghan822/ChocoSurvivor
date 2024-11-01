#include "cprocessing.h"
#include "player.h"

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
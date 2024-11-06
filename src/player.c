#include "cprocessing.h"
#include "player.h"
#include <stdio.h>

// Returns 1 if still alive, 0 if not
int player_take_damage(struct ChocoShooter* player, int damage) {
    player->health = CP_Math_ClampInt(player->health - damage, 0, player->maxHealth);

    if (player->health > 0) {
        printf("Player took %d damage, health is now %d\n", damage, player->health);
        return 1;
    }

    printf("Choco Shooter has been defeated!\n");
    return 0;
}
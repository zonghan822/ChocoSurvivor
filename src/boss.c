#pragma
#include "cprocessing.h"
#include "flicker.h"
#include "boss.h"
#include "player.h"
#include <stdio.h>

void melee_attack(struct Boss boss, struct ChocoShooter *player){
    // Add delay
    printf("Melee attack\n");

    // Damage player
    player_take_damage(player, 1);
}

int can_melee_attack(struct Boss boss, struct ChocoShooter player){
    float minDistance = 125 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
    return CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance;
}

void Damage(struct Boss *boss, int damage){

    if (is_flickering(&bossFlickerState))
    {
        printf("Boss is invincible and cannot take damage right now.\n");
        return; // Skip the rest of the function if flickering
    }

    if (boss->maxHealth > 0)
    {
        boss->maxHealth -= damage;
        if (boss->maxHealth <= 0)
        {
            boss->maxHealth = 0;
            printf("Boss has been defeated!\n");
            boss->img = 0;
        }
        else
        {
            printf("Boss took %d damage, health is now %d\n", damage, boss->maxHealth);
            if (!is_flickering(&bossFlickerState))
            {
                flicker(&bossFlickerState, 1); // Trigger flickering for boss
            }
        }
    }
}
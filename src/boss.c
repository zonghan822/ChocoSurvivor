#pragma
#include "cprocessing.h"
#include "boss.h"
#include <stdio.h>
#include "flicker.h"
#include "game.h"


void Damage(struct Boss* boss, int damage) {

    if (is_flickering(&bossFlickerState)) {
        printf("Boss is invincible and cannot take damage right now.\n");
        return;  // Skip the rest of the function if flickering
    }

    if (boss->maxHealth > 0) {
        boss->maxHealth -= damage;
        if (boss->maxHealth <= 0) {
            boss->maxHealth = 0;
            printf("Boss has been defeated!\n");
            boss->img = 0;
        }
        else {
            printf("Boss took %d damage, health is now %d\n", damage, boss->maxHealth);
            if (!is_flickering(&bossFlickerState)) {
                flicker(&bossFlickerState, 1);  // Trigger flickering for boss
            }
        }
    }
}
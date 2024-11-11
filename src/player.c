#include "cprocessing.h"
#include "player.h"
#include <stdio.h>
#include "flicker.h"
#include "game.h"

enum BuffType {
    ATTACK_UP,
    ATTACK_SPEED_UP,
    HEALTH_UP
};

struct BuffLevel {
    enum BuffType type;   // Current buff type (can be set to the latest buff type obtained)
    int attackP_lvl;      // Attack power level
    int attackS_lvl;      // Attack speed level
    int health_lvl;       // Health level
};

float buffmeterfill = 0.0f;
const float max_buffmeter = 100.0f; // Maximum value of the buff meter
struct BuffLevel playerBuff = { 0 };

void apply_buff(struct ChocoShooter* player, enum BuffType buff) {
    switch (buff) {
    case ATTACK_UP:
        player->attackPower += 5;  // Increase player's attack power
        playerBuff.attackP_lvl++;  // Increase the attackP_lvl in playerBuff
        break;
    case ATTACK_SPEED_UP:
        player->attackSpeed += 1;
        playerBuff.attackS_lvl++;
        break;
    case HEALTH_UP:
        player->health += 10;
        playerBuff.health_lvl++;
        break;
    }
}

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

void update_buff_meter(struct ChocoShooter* player, float dt) {
    // Adjust fill rate to control how fast the bar fills
    buffmeterfill += 5;

    if (buffmeterfill >= max_buffmeter) {
        buffmeterfill = 0;
        enum BuffType randomBuff = rand() % 3;
        apply_buff(player, randomBuff);
    }
}

// Get buff meter fill percentage as an integer (0 to 100)
int get_buff_meter_fill() {
    int update = buffmeterfill ;
    printf("%d\n", update);  // Print percentage to check if it works as expected
    return update;
}
#include "cprocessing.h"
#include <stdio.h>
#include "player.h"
#include "utils.h"
#include "camera.h"
#include "level1.h"
#include "placeables.h"
#include "audio.h"
#include "buff.h"

static const float EXP_INCREASE_MULTIPLIER = 1.35f;
static bool _hasPlayedLevelUpMusic;


// Sidenote: EXP would refer to buff meter
int update_buff_meter(struct ChocoShooter* player, float dt) {
    float expGainedSpeed = 2000.f;
    float expGained = expGainedSpeed * dt;
    player->exp += expGained;
    player->expGained -= expGained;

    printf("Current Exp: %.2f / %d\n", player->exp, player->playerMaxExp);

    // Full or exceeded EXP
    if (player->exp >= (float)player->playerMaxExp) {
        player->playerLevel++;
        if (!_hasPlayedLevelUpMusic) {
            play_sound_advanced(SOUND_MUSIC_LEVEL_UP, 1.f, 0);
            _hasPlayedLevelUpMusic = true;
        }
        // Do nothing if EXP == playerMaxExp
        // But do return the buff
        // Wrap around only if exceeding
        if (player->exp > (float)player->playerMaxExp) {
            // Wrap around
            player->exp -= (float)player->playerMaxExp;
        }
        // increase maxExp
        player->playerMaxExp *= EXP_INCREASE_MULTIPLIER;

        enum BuffType randomBuff = CP_Random_RangeInt(1, 6);
        return randomBuff;
    }
    else if (_hasPlayedLevelUpMusic)
        _hasPlayedLevelUpMusic = false;

    return 0; // Return 0 if no buff is granted
}

void apply_buff(struct ChocoShooter* player, enum BuffType buff, struct Barrier* barrier, struct Ally* allies) {

    switch (buff) {
    case ATTACK_UP:
        player->attackPower += 5;  // Increase player's attack power
        player->buffs.attackPowerLevel++;  // Increase the attackPowerLevel in playerBuff
        printf("Attack Power Level: %d\n", player->buffs.attackPowerLevel);
        if (player->buffs.attackPowerLevel <= 1) {
            assign_buff_to_slot(player, ATTACK_UP);
        }

        break;
    case ATTACK_SPEED_UP:
        player->attackSpeed += 1;
        player->buffs.attackSpeedLevel++;
        printf("Attack Speed Level: %d\n", player->buffs.attackSpeedLevel);
        if (player->buffs.attackSpeedLevel <= 1) {
            assign_buff_to_slot(player, ATTACK_SPEED_UP);
        }
        break;
    case HEALTH_UP:
        player->maxHealth += 10;
        player->buffs.healthLevel++;
        player->health += 5;
        printf("Health Level: %d\n", player->buffs.healthLevel);
        if (player->buffs.healthLevel <= 1) {
            assign_buff_to_slot(player, HEALTH_UP);
        }
        break;

    case HEAL:
        player->health += 10;
        printf("YOU ARE HEALED MY BOY");
        if (player->health > player->maxHealth)
        {
            player->health = player->maxHealth;
        }
        break;
    case ALLY_LVL_UP:
        allies->attackPower += 5;
        player->buffs.allyLevel++;
        printf("allies leveled up");

        break;
    case BARRIER_LVL_UP:
        barrier->health += 10;
        player->buffs.barrierLevel++;
        printf("barrier leveled up");

        break;

    }
}

void activate_buff_from_slot(struct ChocoShooter* player, int slot) {
    //debug
    if (player->buffs.type[slot - 1] == NO_BUFF) {
        printf("Slot %d is empty, cannot activate buff.\n", slot);
        return;
    }

    // Capture the buff type and level from the selected slot before clearing it
    player->activeBuffType = player->buffs.type[slot - 1];

    switch (player->activeBuffType) {
    case ATTACK_UP:
        player->activeBuffLevel = player->buffs.attackPowerLevel;
        printf("Active buff set to ATTACK_UP with level %d\n", player->activeBuffLevel);
        break;
    case ATTACK_SPEED_UP:
        player->activeBuffLevel = player->buffs.attackSpeedLevel;
        printf("Active buff set to ATTACK_SPEED_UP with level %d\n", player->activeBuffLevel);
        break;
    case HEALTH_UP:
        player->activeBuffLevel = player->buffs.healthLevel;
        printf("Active buff set to HEALTH_UP with level %d\n", player->activeBuffLevel);
        break;
    default:
        player->activeBuffLevel = 0;
        printf("No valid buff in slot %d.\n", slot);
        return;
    }

    // Set the slot visually to empty
    player->buffs.type[slot - 1] = NO_BUFF;
    printf("Cleared buff type from slot %d.\n", slot);  // Debug 
}

void assign_buff_to_slot(struct ChocoShooter* player, enum BuffType buffType) {
    for (int i = 0; i < 3; i++) {
        if (player->buffs.type[i] == NO_BUFF) {  // Find the first empty slot
            player->buffs.type[i] = buffType;
            break;
        }
    }
}
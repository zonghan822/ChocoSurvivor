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

BuffList activeBuffs;

// Sidenote: EXP would refer to buff meter
int update_buff_meter(struct ChocoShooter* player, float dt) {
    if (player->expGained == 0) {
        return 0;
    }
    float expGainedSpeed = player->maxHealth;
    float expGained = expGainedSpeed * dt;
    player->exp += min(player->expGained, expGained);
    player->expGained = max(0, player->expGained - expGained);

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

        enum BuffType randomBuff = CP_Random_RangeInt(ATTACK_UP, REFLECT_DMG);
        return randomBuff;
    }
    else if (_hasPlayedLevelUpMusic)
        _hasPlayedLevelUpMusic = false;

    return 0; // Return 0 if no buff is granted
}

void apply_buff(BuffList* activeBuffs, enum BuffType buff, struct ChocoShooter* player, struct Barrier* barrier, struct Ally* allies) {

    bool isNotApplied = true;

    for (int i = 0; i < activeBuffs->buffCount; i++) {
        if (activeBuffs->buffs[i].type == buff) {
            if (buff >= ATTACK_UP && buff <= BARRIER_LVL_UP) {
                activeBuffs->buffs[i].level++;
            }
            else if (buff != HEAL) {
                activeBuffs->buffs[i].timeLeft += BUFF_TIME;
            }
            isNotApplied = false;
            break;
        }
    }

    if (isNotApplied){
        if (buff >= ATTACK_UP && buff <= BARRIER_LVL_UP) {
            Buff temp = {
                .type = buff,
                .isActive = true,
                .isTimerBuff = false,
                .level = 1,
                .timeLeft = 0
            };
            activeBuffs->buffs[activeBuffs->buffCount++] = temp;
        }
        else if (buff != HEAL) {
            Buff temp = {
                .type = buff,
                .isActive = true,
                .isTimerBuff = true,
                .level = 0,
                .timeLeft = BUFF_TIME
            };
            activeBuffs->buffs[activeBuffs->buffCount++] = temp;
        }
    }

    switch (buff) {
    case ATTACK_UP:
        printf("Attack Up!\n");
        player->attackPower += 5;  // Increase player's attack power
        break;
    case ATTACK_SPEED_UP:
        printf("Attack Speed Up!\n");
        player->attackSpeed += 1;
        break;
    case HEALTH_UP:
        printf("Health Up!\n");
        player->maxHealth += 20;
        break;
    case ALLY_LVL_UP:
        printf("Ally Level Up!\n");
        allies->attackPower += 5;
        break;
    case BARRIER_LVL_UP:
        printf("Barrier Level Up!\n");
        barrier->health += 10;
        break;
    case HEAL:
        printf("Health Recover!\n");
        player->health = min(player->health + player->maxHealth/2, player->maxHealth);
        break;
    case SMART_PISTOL:
        printf("Smart Pistol Activated!\n");
        break;
    case FROZING_SHOTS:
        printf("Frozing Shots Activated!\n");
        break;
    case MULTI_SHOTS:
        printf("Multi Shots Activated!\n");
        break;
    case REFLECT_DMG:
        printf("Reflect Damage Activated!\n");
        break;
    }
}

//void activate_buff_from_slot(struct ChocoShooter* player, int slot) {
//    //debug
//    if (player->buffs.type[slot - 1] == NO_BUFF) {
//        printf("Slot %d is empty, cannot activate buff.\n", slot);
//        return;
//    }
//
//    // Capture the buff type and level from the selected slot before clearing it
//    player->activeBuffType = player->buffs.type[slot - 1];
//
//    switch (player->activeBuffType) {
//    case ATTACK_UP:
//        player->activeBuffLevel = player->buffs.attackPowerLevel;
//        printf("Active buff set to ATTACK_UP with level %d\n", player->activeBuffLevel);
//        break;
//    case ATTACK_SPEED_UP:
//        player->activeBuffLevel = player->buffs.attackSpeedLevel;
//        printf("Active buff set to ATTACK_SPEED_UP with level %d\n", player->activeBuffLevel);
//        break;
//    case HEALTH_UP:
//        player->activeBuffLevel = player->buffs.healthLevel;
//        printf("Active buff set to HEALTH_UP with level %d\n", player->activeBuffLevel);
//        break;
//    default:
//        player->activeBuffLevel = 0;
//        printf("No valid buff in slot %d.\n", slot);
//        return;
//    }
//
//    // Set the slot visually to empty
//    player->buffs.type[slot - 1] = NO_BUFF;
//    printf("Cleared buff type from slot %d.\n", slot);  // Debug 
//}

//void assign_buff_to_slot(struct ChocoShooter* player, enum BuffType buffType) {
//    for (int i = 0; i < 3; i++) {
//        if (player->buffs.type[i] == NO_BUFF) {  // Find the first empty slot
//            player->buffs.type[i] = buffType;
//            break;
//        }
//    }
//}
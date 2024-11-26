/******************************************************************************
*@filename		buff.c
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This file is a header file for astar pathfinding algorithm for enemy and boss
movement.
******************************************************************************/

#include "cprocessing.h"
#include <stdio.h>
#include <string.h>
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
    // if no exp to update return and do nothing
    if (player->expGained == 0) {
        return 0;
    }

    float expGainedSpeed = 100.f;
    float expGained = expGainedSpeed * dt;
    player->exp += min(player->expGained, expGained);
    player->expGained = max(0, player->expGained - expGained);

    // Full or exceeded EXP
    if (player->exp >= (float)player->playerMaxExp) {
        player->playerLevel++;
        if (!_hasPlayedLevelUpMusic) {
            play_sound_advanced(SOUND_MUSIC_LEVEL_UP, 1.f, 0);
            _hasPlayedLevelUpMusic = true;
        }
        
        // Wrap around only if exceeding
        if (player->exp > (float)player->playerMaxExp) {
            // Wrap around
            player->exp -= (float)player->playerMaxExp;
        }
        // increase maxExp
        player->playerMaxExp = (int)((float)player->playerMaxExp * (float)EXP_INCREASE_MULTIPLIER);

        // Generate random buff
        enum BuffType randomBuff = CP_Random_RangeInt(ATTACK_UP, REFLECT_DMG);
        return randomBuff; // Return the buff if EXP == playerMaxExp
    }
    else if (_hasPlayedLevelUpMusic)
        _hasPlayedLevelUpMusic = false;

    return 0; // Return 0 if no buff is granted
}

void apply_buff(BuffList* activeBuffs, enum BuffType buff, struct ChocoShooter* player, struct Barrier* barrier, struct Ally* allies) {
    if (buff == NO_BUFF) 
        return;

    bool isNotApplied = true;

    for (int i = 0; i < activeBuffs->buffCount; i++) {
        if (activeBuffs->buffs[i].type == buff) {
            if (buff >= ATTACK_UP && buff <= BARRIER_LVL_UP) {
                activeBuffs->buffs[i].level++;
            }
            else if (buff != HEAL) {
                activeBuffs->buffs[i].timeLeft += BUFF_TIME;
            }
            activeBuffs->buffs[i].renderStage = SHOWING;
            activeBuffs->buffs[i].renderTime = 0.f;
            isNotApplied = false;
            break;
        }
    }

    if (isNotApplied){
        if (buff >= ATTACK_UP && buff <= BARRIER_LVL_UP) {
            Buff temp = {
                .type = buff,
                .isTimerBuff = false,
                .level = 1,
                .timeLeft = 0,
                .renderStage = SHOWING
            };
            activeBuffs->buffs[activeBuffs->buffCount++] = temp;
        }
        else if (buff != HEAL) {
            Buff temp = {
                .type = buff,
                .isTimerBuff = true,
                .level = 0,
                .timeLeft = BUFF_TIME,
                .renderStage = SHOWING
            };
            activeBuffs->buffs[activeBuffs->buffCount++] = temp;
        }
        else {
            Buff temp = {
                .type = buff,
                .isTimerBuff = true,
                .level = 0,
                .timeLeft = BUFF_SHOW_TIME,
                .renderStage = SHOWING
            };
            activeBuffs->buffs[activeBuffs->buffCount++] = temp;
        }
    }


    switch (buff) {
    case ATTACK_UP:
        printf("Attack Up!\n");
        player->attackPower += 5;  // Increase player's attack power
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Attack Up";
        }
        break;
    case ATTACK_SPEED_UP:
        printf("Attack Speed Up!\n");
        player->attackSpeed += 1;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atk_spd_up.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Attack Speed Up";
        }
        break;
    case HEALTH_UP: {
        printf("Health Up!\n");
        float healthPercent = (float)player->health / (float)player->maxHealth;
        player->maxHealth += 20;
        player->health = (int)(healthPercent * (float)player->maxHealth);
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/hpup.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Health Up";
        }
        break;
    }   
    case ALLY_LVL_UP:
        printf("Sentry Level Up!\n");
        allies->attackPower += ALLY_ATTACK_POWER_INCREMENT;
        allies->attackSpeed += ALLY_ATTACK_SPEED_INCREMENT;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/sentry_lvl_up.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Sentry Level Up";
        }
        break;
    case BARRIER_LVL_UP:
        printf("Barricade Level Up!\n");
        barrier->health += 10;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/barricade_lvl_up.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Barricade Level Up";
        }
        break;
    case HEAL:
        printf("Health Recover!\n");
        player->health = min(player->health + player->maxHealth/2, player->maxHealth);
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/heal.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Health Recover";
        }
        break;
    case SMART_PISTOL:
        printf("Smart Gun Activated!\n");
        player->currentProjectileType |= TRACKING_PROJECTILE;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/smartgun.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Smart Gun";
        }
        break;
    case FROZING_SHOTS:
        printf("Freezing Shots Activated!\n");
        player->currentProjectileType |= FREEZING_PROJECTILE;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/frozingshots.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Freezing Shots";
        }
        break;
    case MULTI_SHOTS:
        printf("Multi Shots Activated!\n");
        player->currentProjectileType |= FAST_PROJECTILE;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/fastshots.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Fast Shots";
        }
        break;
    case REFLECT_DMG:
        printf("Reflect Damage Activated!\n");
        player->isReflectingDmg = true;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/reflectdmg.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Reflect Damage";
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderTime /= 3.25f;
        }
        break;
    }
}

void update_timer_buff(BuffList* activeBuffs, float dt) {
    for (int i = 0; i < activeBuffs->buffCount; i++) {
        if (activeBuffs->buffs[i].isTimerBuff) {
            activeBuffs->buffs[i].timeLeft -= dt;
            if (activeBuffs->buffs[i].timeLeft <= 0) {
                switch (activeBuffs->buffs[i].type) {
                case SMART_PISTOL:
                    playerInstance.currentProjectileType ^= TRACKING_PROJECTILE;
                    break;
                case FROZING_SHOTS:
                    playerInstance.currentProjectileType ^= FREEZING_PROJECTILE;
                    break;
                case MULTI_SHOTS:
                    playerInstance.currentProjectileType ^= FAST_PROJECTILE;
                    break;
                case REFLECT_DMG:
                    playerInstance.isReflectingDmg = false;
                    break;
                }
                for (int j = i; j < activeBuffs->buffCount; j++) {
                    Buff temp = activeBuffs->buffs[j];
                    activeBuffs->buffs[j] = activeBuffs->buffs[j + 1];
                    activeBuffs->buffs[j + 1] = temp;
                }
                activeBuffs->buffCount--;
                i--;
            }
        }
    }
}

void assign_buff_to_slot(BuffSlot* buffSlot, BuffType buff) {
    for (int i = 0; i < MAX_BUFF_SLOT; i++) {
        if (!buffSlot->buffs[i]) {
            buffSlot->buffs[i] = buff;
            buffSlot->buffCount++;
            break;
        }
    }
}

void render_buff_slot(BuffSlot* buffSlot, float windowWidth, float windowHeight, const CP_Image* inputImgs) {
    float slotX[MAX_BUFF_SLOT] = { windowWidth / 10.f };
    float slotY = windowHeight * 5.5f / 6.5f;
    float slotW = BUFF_ICON_SIDE, slotH = BUFF_ICON_SIDE;
    float gap = 40.f;

    CP_Image img;

    for (int i = 0; i < MAX_BUFF_SLOT; i++) {
        slotX[i] = (i * (gap + slotW)) + slotX[0];
        float slotInputY = slotY / 1.1f,
            slotInputW = slotW / 1.75f, slotInputH = slotH / 1.75f;
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 225));
        CP_Graphics_DrawRectAdvanced(slotX[i], slotInputY, slotInputW - 10.f, slotInputH - 10.f, 0.f, 6.f);
        CP_Image_Draw(inputImgs[i], slotX[i], slotInputY, slotInputW, slotInputH, 255);
        switch (buffSlot->buffs[i]) {
        case NO_BUFF:
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRect(slotX[i], slotY, slotW, slotH);
            break;
        case HEAL:
            CP_Image_Draw(img = CP_Image_Load("Assets/heal.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case SMART_PISTOL:
            CP_Image_Draw(img = CP_Image_Load("Assets/smartgun.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case FROZING_SHOTS:
            CP_Image_Draw(img = CP_Image_Load("Assets/frozingshots.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case MULTI_SHOTS:
            CP_Image_Draw(img = CP_Image_Load("Assets/fastshots.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case REFLECT_DMG:
            CP_Image_Draw(img = CP_Image_Load("Assets/reflectdmg.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        }  
    }
}

void render_buff(BuffList* activeBuffs, float windowWidth, float windowHeight, float dt) {
    int showCount = 0;
    for (int j = 0; j < activeBuffs->buffCount; j++) {
        if (activeBuffs->buffs[j].renderStage == SHOWING) {
            showCount++;
        }
    }
    int showIndex = 0;
    float showGap = BUFF_ICON_SIDE * 2;
    float showXfirst = (windowWidth / 2.f) - ((((float)showCount - 1.f) / 2.f) * ((float)BUFF_ICON_SIDE + showGap));
    /*if (showCount >= 1) {
        printf("buff display count: %d, first x : %lf\n", showCount, showXfirst);
    }*/

    for (int i = 0; i < activeBuffs->buffCount; i++) {
        activeBuffs->buffs[i].renderTime += dt;

        switch (activeBuffs->buffs[i].renderStage) {
        case SHOWING: {
            activeBuffs->buffs[i].renderPos.x = showXfirst + (showIndex++ * (BUFF_ICON_SIDE + showGap));
            activeBuffs->buffs[i].renderPos.y = windowHeight / 4.f;
            activeBuffs->buffs[i].renderSide = BUFF_ICON_SIDE;

            float textPosX = activeBuffs->buffs[i].renderPos.x;
            float textPosY = activeBuffs->buffs[i].renderPos.y - (activeBuffs->buffs[i].renderSide / 2) - 20.f;

            CP_Image_Draw(activeBuffs->buffs[i].img,
                activeBuffs->buffs[i].renderPos.x,
                activeBuffs->buffs[i].renderPos.y,
                activeBuffs->buffs[i].renderSide,
                activeBuffs->buffs[i].renderSide, 255);

            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
            float textSize = showCount == 1  ? 50.f : 40.f;
            CP_Settings_TextSize(textSize);
            CP_Font_DrawText(activeBuffs->buffs[i].renderText, textPosX, textPosY);
            if (activeBuffs->buffs[i].renderTime >= BUFF_SHOW_TIME)
                activeBuffs->buffs[i].renderStage = MOVING;
            break;
        }
        case MOVING: {
            float moveProgress = min(1, activeBuffs->buffs[i].renderTime - 2);
            float destSide = BUFF_ICON_SIDE / 2;
            float gap = 5.f;
            float destX = (i * (gap + destSide)) + (windowWidth / 10 * .75f); // (windowWidth / 8 * 10);
            float destY = 110.f + 20.f; // 110 is info panel y                // windowHeight * 14.f / 15.f;

            CP_Vector destPos = { destX, destY };

            activeBuffs->buffs[i].renderPos.x = CP_Math_LerpFloat(activeBuffs->buffs[i].renderPos.x, destX, moveProgress);
            activeBuffs->buffs[i].renderPos.y = CP_Math_LerpFloat(activeBuffs->buffs[i].renderPos.y, destY, moveProgress);
            activeBuffs->buffs[i].renderSide = CP_Math_LerpFloat(activeBuffs->buffs[i].renderSide, destSide, moveProgress);

            CP_Image_Draw(activeBuffs->buffs[i].img,
                activeBuffs->buffs[i].renderPos.x,
                activeBuffs->buffs[i].renderPos.y,
                activeBuffs->buffs[i].renderSide,
                activeBuffs->buffs[i].renderSide, 255);

            if (moveProgress == 1)
                activeBuffs->buffs[i].renderStage = STAYING;
            break;
        }
        case STAYING: {
            float gap = 5.f;
            float x = (i * (gap + activeBuffs->buffs[i].renderSide)) + (windowWidth / 10 * .75f); // (windowWidth / 8 * 10);
            CP_Image_Draw(activeBuffs->buffs[i].img,
                x, activeBuffs->buffs[i].renderPos.y,
                activeBuffs->buffs[i].renderSide,
                activeBuffs->buffs[i].renderSide, 255);
            if (activeBuffs->buffs[i].isTimerBuff) {
                CP_Settings_Fill(CP_Color_Create(0, 0, 0, 235));
                CP_Settings_TextSize(32.f);
                char timerBuf[5];
                sprintf_s(timerBuf, sizeof(timerBuf), "%d", (int)(activeBuffs->buffs[i].timeLeft));
                CP_Font_DrawText(timerBuf, x, activeBuffs->buffs[i].renderPos.y + 37.5f);
            }
            else {
                CP_Settings_Fill(CP_Color_Create(0, 0, 0, 235));
                CP_Settings_TextSize(20.f);
                char lvlBuf[10];
                sprintf_s(lvlBuf, sizeof(lvlBuf), "Lvl. %d", (int)(activeBuffs->buffs[i].level));
                CP_Font_DrawText(lvlBuf, x, activeBuffs->buffs[i].renderPos.y + 37.5f);
            }
            break;
        }      
        }
    }
}
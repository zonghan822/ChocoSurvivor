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
    if (player->expGained == 0) {
        return 0;
    }
    float expGainedSpeed = player->maxHealth;
    float expGained = expGainedSpeed * dt;
    player->exp += min(player->expGained, expGained);
    player->expGained = max(0, player->expGained - expGained);

    // printf("Current Exp: %.2f / %d\n", player->exp, player->playerMaxExp);

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
                .timeLeft = 2.f,
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
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Attack Speed Up";
        }
        break;
    case HEALTH_UP:
        printf("Health Up!\n");
        player->maxHealth += 20;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Health Up";
        }
        break;
    case ALLY_LVL_UP:
        printf("Ally Level Up!\n");
        allies->attackPower += 5;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Ally Level Up";
        }
        break;
    case BARRIER_LVL_UP:
        printf("Barrier Level Up!\n");
        barrier->health += 10;
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Barrier Level Up";
        }
        break;
    case HEAL:
        printf("Health Recover!\n");
        player->health = min(player->health + player->maxHealth/2, player->maxHealth);
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Health Recover";
        }
        break;
    case SMART_PISTOL:
        printf("Smart Pistol Activated!\n");
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Tracking Shots";
        }
        break;
    case FROZING_SHOTS:
        printf("Frozing Shots Activated!\n");
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Frozing Shots";
        }
        break;
    case MULTI_SHOTS:
        printf("Multi Shots Activated!\n");
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Multi Shots";
        }
        break;
    case REFLECT_DMG:
        printf("Reflect Damage Activated!\n");
        if (isNotApplied) {
            activeBuffs->buffs[activeBuffs->buffCount - 1].img = CP_Image_Load("Assets/atkbuff.png");
            activeBuffs->buffs[activeBuffs->buffCount - 1].renderText = "Reflect Damage";
        }
        break;
    }
}

void update_timer_buff(BuffList* activeBuffs, float dt) {
    for (int i = 0; i < activeBuffs->buffCount; i++) {
        if (activeBuffs->buffs[i].isTimerBuff) {
            activeBuffs->buffs[i].timeLeft -= dt;
            if (activeBuffs->buffs[i].timeLeft <= 0) {
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

void render_buff_slot(BuffSlot* buffSlot, float windowWidth, float windowHeight) {
    float slotX[MAX_BUFF_SLOT] = { windowWidth / 10.f };
    float slotY = windowHeight * 9.f / 10.f;
    float slotW = BUFF_ICON_SIDE, slotH = BUFF_ICON_SIDE;
    float gap = 40.f;

    CP_Image img;

    for (int i = 0; i < MAX_BUFF_SLOT; i++) {
        slotX[i] = (i * (gap + slotW)) + slotX[0];
        switch (buffSlot->buffs[i]) {
        case NO_BUFF:
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRect(slotX[i], slotY, slotW, slotH);
            break;
        case HEAL:
            CP_Image_Draw(img = CP_Image_Load("Assets/atkbuff.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case SMART_PISTOL:
            CP_Image_Draw(img = CP_Image_Load("Assets/atkbuff.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case FROZING_SHOTS:
            CP_Image_Draw(img = CP_Image_Load("Assets/atkbuff.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case MULTI_SHOTS:
            CP_Image_Draw(img = CP_Image_Load("Assets/atkbuff.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        case REFLECT_DMG:
            CP_Image_Draw(img = CP_Image_Load("Assets/atkbuff.png"), slotX[i], slotY, slotW, slotH, 255);
            break;
        }  
    }
}

void render_buff(BuffList* activeBuffs, float windowWidth, float windowHeight, float dt) {
    for (int i = 0; i < activeBuffs->buffCount; i++) {
        activeBuffs->buffs[i].renderTime += dt;

        switch (activeBuffs->buffs[i].renderStage) {
        case SHOWING:
            activeBuffs->buffs[i].renderPos.x = windowWidth / 2.f;
            activeBuffs->buffs[i].renderPos.y = windowHeight / 3.f;
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
            CP_Settings_TextSize(50.f);
            CP_Font_DrawText(activeBuffs->buffs[i].renderText, textPosX, textPosY);
            if (activeBuffs->buffs[i].renderTime >= 2)
                activeBuffs->buffs[i].renderStage = MOVING;
            break;

        case MOVING: {
            float moveProgress = min(1, activeBuffs->buffs[i].renderTime - 2);
            float destSide = BUFF_ICON_SIDE / 2;
            float gap = 5.f;
            float destX = (i * (gap + destSide)) + (windowWidth * 8 / 10);
            float destY = windowHeight * 14.f / 15.f;

            CP_Vector destPos = { destX, destY };
            CP_Vector moveDirection = (CP_Vector_Subtract(destPos, activeBuffs->buffs[i].renderPos));

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
            CP_Image_Draw(activeBuffs->buffs[i].img,
                (i * (gap + activeBuffs->buffs[i].renderSide)) + (windowWidth * 8 / 10),
                activeBuffs->buffs[i].renderPos.y,
                activeBuffs->buffs[i].renderSide,
                activeBuffs->buffs[i].renderSide, 255);
            break;
        }      
        }
    }
}
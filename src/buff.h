#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "player.h"

#define BUFF_TIME 20.f
#define MAX_BUFF_SLOT 3
#define BUFF_ICON_SIDE 80.f

typedef enum BuffType {
    NO_BUFF,
    ATTACK_UP,
    ATTACK_SPEED_UP,
    HEALTH_UP,
    ALLY_LVL_UP,
    BARRIER_LVL_UP,
    HEAL,
    SMART_PISTOL,
    FROZING_SHOTS,
    MULTI_SHOTS,
    REFLECT_DMG
}BuffType;

typedef enum BuffRenderStage {
    NOT_RENDERING,
    SHOWING,
    MOVING,
    STAYING
}BuffRenderStage;

typedef struct Buff {
    BuffType type;
    bool isTimerBuff;
    float timeLeft;
    int level;
    CP_Image img;
    BuffRenderStage renderStage;
    CP_Vector renderPos;
    float renderSide;
    char *renderText;
    float renderTime;
}Buff;

typedef struct {
    enum BuffType buffs[MAX_BUFF_SLOT];
    int buffCount;
}BuffSlot;

typedef struct {
    Buff buffs[10];
    int buffCount;
}BuffList;

extern BuffList activeBuffs;

void apply_buff(BuffList *activeBuffs, enum BuffType buff, struct ChocoShooter* player, struct Barrier* barrier, struct Ally* allies);
int update_buff_meter(struct ChocoShooter* player, float dt);
void update_timer_buff(BuffList* activeBuffs, float dt);
void assign_buff_to_slot(BuffSlot* buffSlot, BuffType buff);
void render_buff_slot(BuffSlot* buffSlot, float windowWidth, float windowHeight);
void render_buff(BuffList* activeBuffs, float windowWidth, float windowHeight, float dt);
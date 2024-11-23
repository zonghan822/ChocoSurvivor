#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "player.h"

#define BUFF_TIME 10.f

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

typedef struct Buff {
    BuffType type;
    bool isActive;
    bool isTimerBuff;
    float timeLeft;
    int level;
}Buff;

typedef struct {
    Buff buffs[10];
    int buffCount;
}BuffList;

extern BuffList activeBuffs;

void apply_buff(BuffList *activeBuffs, enum BuffType buff, struct ChocoShooter* player, struct Barrier* barrier, struct Ally* allies);
int update_buff_meter(struct ChocoShooter* player, float dt);
//void activate_buff_from_slot(struct ChocoShooter* player, int slot);
//void assign_buff_to_slot(struct ChocoShooter* player, enum BuffType buffType);
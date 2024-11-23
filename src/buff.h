#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "player.h"

typedef enum BuffType {
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
    bool isTimerBuff;
    bool isActiveBuff;
    float timeLeft;
    int level;
}Buff;

typedef struct {
    Buff buffs[5];
    int buffCount;
}BuffList;

void apply_buff(struct ChocoShooter* player, enum BuffType buff, struct Barrier* barrier, struct Ally* allies);
int update_buff_meter(struct ChocoShooter* player, float dt);
void activate_buff_from_slot(struct ChocoShooter* player, int slot);
void assign_buff_to_slot(struct ChocoShooter* player, enum BuffType buffType);
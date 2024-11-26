/******************************************************************************
*@filename		buff.h
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This file is a header file for astar pathfinding algorithm for enemy and boss
movement.
******************************************************************************/

#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "player.h"

#define BUFF_TIME 20.f
#define MAX_BUFF_SLOT 3
#define BUFF_ICON_SIDE 80.f
#define BUFF_SHOW_TIME 2.F

#define ALLY_ATTACK_POWER_INCREMENT 3
#define ALLY_ATTACK_SPEED_INCREMENT 0.3f

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
void render_buff_slot(BuffSlot* buffSlot, float windowWidth, float windowHeight, const CP_Image* inputImgs);
void render_buff(BuffList* activeBuffs, float windowWidth, float windowHeight, float dt);
int debug_assign_buff(void);
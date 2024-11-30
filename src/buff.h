/******************************************************************************
*@filename		buff.h
*@author(s)		Law Zong Han, Dalton Koh
*@DP emails		zonghan.law@digipen.edu, d.koh@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This header file declares functions and structures for handling the buff feature.
The buff type and the buff rendering stage are defined in enums as effective 
indicator for usage in buff.c. Functions to update the buff meter, buff slots,
activated buffs, rendering and debuging are provided in this file.
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
#define ALLY_ATTACK_SPEED_INCREMENT 0.5f

/*****************************
     Made by Law Zong Han
******************************/
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


/****************************************
   Made by Dalton Koh and Law Zong Han
******************************************/
int update_buff_meter(struct ChocoShooter* player, float dt);
void apply_buff(BuffList* activeBuffs, enum BuffType buff, struct ChocoShooter* player, struct Barrier* barriers, struct Ally* allies);


/*****************************
      Made by Dalton Koh
******************************/
void assign_buff_to_slot(BuffSlot* buffSlot, BuffType buff);
void render_buff_slot(BuffSlot* buffSlot, float windowWidth, float windowHeight, const CP_Image* inputImgs);


/*****************************
     Made by Law Zong Han
******************************/
void init_buff_slot_img(void);
void update_timer_buff(BuffList* activeBuffs, float dt);
void render_buff(BuffList* activeBuffs, float windowWidth, float windowHeight, float dt);
int get_buff_level(enum BuffType buff);
void dispose_buff(void);
int debug_assign_buff(void);
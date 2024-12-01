/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
* @filename		placeables.h
* @author(s)	Dalton koh
* @DP emails	d.koh@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file declares the functions necessary for the creation of barriers and allies 
                . Which also entails the attributes of the two kind of placeables and its initial values.
                The main features and rendering of the placeables are also detailed here.
******************************************************************************/

#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"

// Barrier macros
#define MAX_BARRIERS 3
#define BARRIER_INITIAL_HEALTH 200
#define BARRIER_HEALTHBAR_DISPLAY_TIME 3.f
#define MAX_BARRIER_IMAGES 4
#define BARRIER_WIDTH 100.f
#define BARRIER_HEIGHT 150.f
#define BARRIER_RECT_WIDTH 50.f
#define BARRIER_RECT_HEIGHT 50.f

#define MAX_ALLIES 3
#define MAX_ALLIES_IMAGES 4
#define ALLY_WIDTH 100.f
#define ALLY_HEIGHT 150.f

struct Barrier {
    CP_Vector pos;
    float width;
    float height;
    int health;
    int maxHealth;
    struct Rectangle hitbox;
    bool isTakingDamage;
    float healthBarDisplayTime;
};

enum BarrierState {
    BARRIER_IDLE,
    BARRIER_PREVIEW
};

extern struct Barrier barriers[MAX_BARRIERS];
extern int barrierCount;

#pragma region Barrier
// Functions for managing barriers
int barrier_take_damage(int barrierIndex, int damage);
void init_barrier_images(void);
CP_Image get_barrier_image(int barrierLevel);
int get_barrier_count(void);
void increment_barrier_count(void);
void set_barrier_properties(int index, CP_Vector pos, float width, float height, int health);
void render_barriers();
void dispose_barriers(void);
void sort_barriers(void);
#pragma endregion

extern struct Ally allies[MAX_ALLIES];
extern int allyCount;

enum AllyState {
    ALLY_IDLE = 0,
    ALLY_PREVIEW = 1,
};

struct Ally {
    CP_Vector pos;
    float width;
    float height;
    float attackSpeed;
    int attackPower;
    enum AllyState state;
};

#pragma region Allies
// Functions for managing allies
void init_ally_images(void);
void init_ally(int index, CP_Vector pos, float width, float height);
CP_Image get_ally_image(int allyLevel);
int get_ally_count(void);
void increment_ally_count(void);
void render_allies();
void dispose_allies(void);
#pragma endregion

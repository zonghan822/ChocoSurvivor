/******************************************************************************
*@filename		enemy.h
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
#include "utils.h"
#include "player.h"
#include "tower.h"
#include "astar.h"

#define ENEMY_WIDTH 60.f
#define ENEMY_HITBOX_OFFSET_X(enemyWidth) (- 0.2f * enemyWidth)
#define ENEMY_HITBOX_OFFSET_Y(enemyHeight) (-0.08f * enemyHeight)
#define ENEMY_HEALTHBAR_DISPLAY_TIME 3.f
#define ROACH_FRAMES 6

enum EnemyState {
	ENEMY_STATE_DIED = 0,
	ENEMY_STATE_MOVING_TO_TOWER,
	ENEMY_STATE_MOVING_TO_PLAYER,
	ENEMY_STATE_DASHING,
	ENEMY_STATE_ATTACKING,
	ENEMY_STATE_IDLE
};

struct ChocoRoach {
    CP_Image roachFrames[ROACH_FRAMES];
    CP_Vector pos;
    float width;
    float height;
    float speed;
    int health;
    int maxHealth;
    int dmg;
    float attackCooldown;
    CP_Vector moveDirection;
    enum EnemyState currentState;
    struct Rectangle hitbox;
    bool isTakingDamage;
    float healthBarDisplayTime;
    int barrierAttack;
    int currentFrame; // Index of the current frame being rendered
    float frameTime;  // Time per frame
    float timeAccumulator;
    int previousNode;
    AStarPath path;
};

// Takes in single enemy
struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float spawnWidth, float speed, int maxHealth, int dmg, float frameTime);
void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount);
int enemy_take_damage(struct ChocoRoach* roach, int dmg);

// Takes in array of enemies
void render_enemies(struct ChocoRoach* roaches, int size, float deltaTime);
void sort_enemies(struct ChocoRoach* roachesArr, int size);

/******************************************************************************
* @filename		boss.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file defines the logic for handling boss characters in the
				game, including movement, attacks, health, and damage. It
				implements functions for the first boss (`boss_one`), such as
				primary and secondary attacks, movement using A* pathfinding,
				and state transitions (e.g., moving to the player or tower).
				Private variables manage attack delays and cooldowns.
				The file also includes a function to spawn and render the boss,
				as well as handle damage and attack effects.
				Unused code for a second boss (`boss_two`) is commented out.
******************************************************************************/

#pragma
#include "cprocessing.h"
#include "boss.h"
#include "player.h"
#include "utils.h"
#include "hittest.h"
#include "astar.h"
#include "tower.h"
#include "placeables.h"
#include "enemy.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#define M_PI 3.1415927f

// ************* Private variables ************* //
//         _xAtkDelay is for pre-damage          //
//      _xAttackCooldown is for post-damage      //
//       Unless specified in the function        //
// ********************************************* //
static float _priAtkDelay = 0.f, _priAtkCooldown = 0.f;
static float _secAtkDelay = 0.f, _secAtkCooldown = 0.f;

// Boss one attack private variables
static float _chargeProgress, _chargeHasAttackedPlayer;
static CP_Vector _meleeOneVec, _chargeTo;
static const float BOSS_ONE_PRI_ATK_DISTANCE = 150.f;
static const float BOSS_ONE_SEC_ATK_DISTANCE = 275.f;

// Boss two attack private variables
static CP_Vector _meleeTwoVec;
static float _previousPlayerSpeed = 0.f;

void dispose_boss(struct Boss* boss) {
	if (boss == NULL) return;
	boss->hitbox.pos = boss->pos = CP_Vector_Set(-500.f, -500.f);
	CP_Image_Free(&boss->img);
}

static void _move_to_target(struct Boss* boss, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
	if (!boss->path.hasUpdated) {
		int startNodeIndex = get_nearest_node_index(grid, nodeCount, boss->pos);
		int endNodeIndex = get_nearest_node_index(grid, nodeCount, hitbox.pos);
		if (startNodeIndex == endNodeIndex) {
			boss->path.paths[0] = endNodeIndex;
			boss->path.pathNodeCount = 1;
		}
		else {
			boss->path.pathNodeCount = find_astar_path(grid, nodeCount, startNodeIndex, endNodeIndex, boss->path.paths);
			if (boss->path.pathNodeCount == 0) {
				int startNodeIndexNB = get_nearest_node_index(gridNoBarrier, nodeCount, boss->pos);
				int endNodeIndexNB = get_nearest_node_index(gridNoBarrier, nodeCount, hitbox.pos);
				boss->path.pathNodeCount = find_astar_path(gridNoBarrier, nodeCount, startNodeIndexNB, endNodeIndexNB, boss->path.paths);
			}
		}
		boss->path.hasUpdated = true;
		printf("Boss path updated!\n");
	}

	if (boss->path.pathNodeCount) {
		if ((CP_Vector_Distance(grid[boss->path.paths[0]].pos, boss->pos) < 2.5f) || boss->previousNode == boss->path.paths[0]) {
			boss->previousNode = boss->path.paths[0];
			pop_int_array(boss->path.paths, &(boss->path.pathNodeCount), boss->path.paths[0]);
		}
	}

	if (boss->path.pathNodeCount)
		boss->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(grid[boss->path.paths[0]].pos, boss->pos)));
	else
		boss->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(hitbox.pos, boss->pos)));

	boss->width = (boss->moveDirection.x < 0.f && boss->width > 0.f) || (boss->moveDirection.x > 0.f && boss->width < 0.f) ? -boss->width : boss->width;
	boss->pos.x += boss->moveDirection.x * boss->speed * dt;
	boss->pos.y += boss->moveDirection.y * boss->speed * dt;

	//CP_Vector hitboxPos = { boss->pos.x + BOSS_HITBOX_OFFSET_X(boss->width), boss->pos.y};
	update_rect_hitbox(&boss->hitbox, boss->width, boss->pos, BOSS_HITBOX_OFFSET_X(boss->width), 0.f);
}

struct Boss init_boss_one(float width, float speed, int health, int attack_power, float attack_speed) {
	CP_Image img = CP_Image_Load("Assets/Sprites/boss_1.png");
	float imgWidth = ((float)CP_Image_GetWidth(img)),
		imgHeight = ((float)CP_Image_GetHeight(img));
	float preservedHeight = get_preserved_heightf(imgWidth, imgHeight, width);
	_secAtkDelay = _secAtkCooldown = _priAtkDelay = _priAtkCooldown = 0.f;
	_chargeTo = _meleeOneVec = CP_Vector_Zero();
	_chargeProgress = 0.f;
	struct Boss boss = {
		.img = img,
		.speed = speed,
		.pos = CP_Vector_Set(-500.f, -500.f),
		.health = health,
		.maxHealth = health,
		.width = width,
		.height = preservedHeight,
		.state = BOSS_STATE_UNSPAWNED,
		.attackPower = attack_power,
		.attackSpeed = attack_speed,
		.primaryAttack = boss_one_pri_atk,
		.canPrimaryAttack = can_boss_one_pri_atk,
		.secondaryAttack = boss_one_sec_atk,
		.canSecondaryAttack = can_boss_one_sec_atk,
		.move = boss_one_move,
		.previousNode = -1,
		.path.pathNodeCount = 0,
		.path.hasUpdated = false
	};
	boss.hitbox.pos.x = boss.pos.x + BOSS_HITBOX_OFFSET_X(boss.width);
	boss.hitbox.pos.y = boss.pos.y;
	boss.hitbox.width = 77.f / 144.f * boss.width;
	boss.hitbox.height = 166.f / 251.f * boss.height;

	return boss;
}

/*
struct Boss init_boss_two(float width, float speed, int health, int attack_power, float attack_speed) {
	CP_Image img = CP_Image_Load("Assets/boss_lvl2.png");
	float preservedHeight = get_preserved_heightf(CP_Image_GetWidth(img), CP_Image_GetHeight(img), width);
	struct Boss boss = {
		.img = img,
		.speed = speed,
		.pos = CP_Vector_Set(-500.f, -500.f),
		.hitbox.pos = CP_Vector_Set(-500.f, -500.f),
		.health = health,
		.maxHealth = health,
		.width = width,
		.height = preservedHeight,
		.state = BOSS_STATE_UNSPAWNED,
		.hitbox.width = boss.width,
		.hitbox.height = boss.height,
		.attackPower = attack_power,
		.attackSpeed = attack_speed,
		.primaryAttack = boss_two_pri_atk,
		.canPrimaryAttack = can_boss_two_pri_atk,
		.secondaryAttack = boss_two_sec_atk,
		.canSecondaryAttack = can_boss_two_sec_atk,
		.move = boss_two_move
	};

	return boss;
}
*/

void render_boss(struct Boss boss) {
	CP_Image_Draw(boss.img, boss.pos.x, boss.pos.y, boss.width, boss.height, 255);
}

void spawn_boss(struct Boss* boss, float x, float y) {
	CP_Vector vec = CP_Vector_Set(x, y);
	boss->pos = vec;
	boss->state = BOSS_STATE_MOVING_TO_TOWER;
}

int boss_take_damage(struct Boss* boss, int damage) {
	boss->health = CP_Math_ClampInt(boss->health - damage, 0, boss->maxHealth);
	boss->isTakingDamage = true;

	if (boss->health <= 0) {
		printf("Boss has been defeated!\n");
		boss->state = BOSS_STATE_DIED;
		return 1;
	}

	printf("Boss took %d damage, health is now %d\n", damage, boss->health);
	return 0;
}

#pragma region Boss 1
void boss_one_pri_atk(struct Boss *boss, struct ChocoShooter* player, struct DefenseTarget* tower, float dt) {
	// _priAtkCooldown in this func is used
	// for the boss recovering from attack (back to idle animation)
	boss->state = BOSS_STATE_ATTACKING_PRIMARY;
	const float attackingDelay = 2.7f,
		postAttackDelay = 1.8f;

	struct Rectangle towerHitbox = get_tower_hitbox(*tower);
	if (_meleeOneVec.x == 0 && _meleeOneVec.y == 0) {
		printf("Boss is performing melee attack\n");
		float minDistance = BOSS_ONE_PRI_ATK_DISTANCE + (fabsf(boss->hitbox.width) / 2.f);

		// Melee on the tower pos
		if (CP_Vector_Distance(boss->hitbox.pos, towerHitbox.pos) <= minDistance + (towerHitbox.width / 2.f))
			_meleeOneVec = towerHitbox.pos;
		// Melee on player pos
		else if (CP_Vector_Distance(boss->hitbox.pos, player->hitbox.pos) <= minDistance + (fabsf(player->hitbox.width) / 2.f))
			_meleeOneVec = player->pos;
	}

	// Add delay (attacking animation)
	if (_priAtkDelay < attackingDelay) {
		_priAtkDelay += dt * boss->attackSpeed;
		return;
	}

	// Damage player entities if conditions meet
	if (_priAtkCooldown == 0.f) {
		float distanceToPlayer = CP_Vector_Distance(_meleeOneVec, player->hitbox.pos),
			distanceToTower = CP_Vector_Distance(_meleeOneVec, towerHitbox.pos);

		if (distanceToPlayer <= BOSS_ONE_PRI_ATK_DISTANCE)
			if (playerInstance.isReflectingDmg)
				boss_take_damage(boss, boss->attackPower);
			else
				player_take_damage(player, boss->attackPower);
		if (distanceToTower <= BOSS_ONE_PRI_ATK_DISTANCE)
			tower_take_damage(tower, 1);
		printf("Boss has performed melee attack\n");
	}

	// Post attack animation
	if (_priAtkCooldown < postAttackDelay)
	{
		_priAtkCooldown += dt;
		return;
	}

	printf("Boss has recovered from melee attack\n");
	boss->state = BOSS_STATE_MOVING_TO_TOWER;
	_priAtkDelay = _priAtkCooldown = 0.f;
	_meleeOneVec = CP_Vector_Zero();
}


void render_boss_one_pri_atk_telegraph() {
	if (_priAtkDelay == 0.f || _priAtkCooldown != 0.f) return;

	CP_Settings_Fill(CP_Color_Create(255, 0, 0, 75));

	CP_Graphics_DrawCircle(_meleeOneVec.x, _meleeOneVec.y, BOSS_ONE_PRI_ATK_DISTANCE * 1.85f);
}

int can_boss_one_pri_atk(struct Boss boss, struct ChocoShooter player, struct DefenseTarget tower) {
	// Min range to snapshot the player's pos for the attack,
	// then add half width (distance from center origin)
	float minDistance = BOSS_ONE_PRI_ATK_DISTANCE + (fabsf(boss.hitbox.width) / 2.f);
	struct Rectangle towerHitbox = get_tower_hitbox(tower);
	return ((CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance + (fabsf(player.hitbox.width) / 2.f)
		|| (CP_Vector_Distance(boss.hitbox.pos, towerHitbox.pos) <= minDistance + (towerHitbox.width / 2.f) 
		|| boss.state == BOSS_STATE_ATTACKING_PRIMARY)) && boss.state != BOSS_STATE_ATTACKING_SECONDARY);
}

// Attack only the player
// But if the tower is within the area, damage it too
void boss_one_sec_atk(struct Boss *boss, struct ChocoShooter* player, struct DefenseTarget* tower, float dt) {
	boss->state = BOSS_STATE_ATTACKING_SECONDARY;
	const float attackingDelay = 3.5f;
	printf("Boss is performing charge attack\n");

	const float maxDistance = (fabsf(boss->hitbox.width) / 2) + BOSS_ONE_SEC_ATK_DISTANCE;

	// Set its charge to pos to player's
	if (_chargeTo.x == 0 && _chargeTo.y == 0) {
		_chargeTo = player->hitbox.pos;

		if (_chargeTo.x < boss->pos.x && boss->width > 0) // Flip
			boss->width = -boss->width;
	}

	// Add delay (attacking animation)
	if (_secAtkDelay < attackingDelay) {
		_secAtkDelay += dt * boss->attackSpeed;
		return;
	}

	// Lerp
	// Use speed instead
	_chargeProgress += (boss->speed * dt) / 15.f;
	boss->pos.x = CP_Math_LerpFloat(boss->pos.x, _chargeTo.x, _chargeProgress);
	boss->pos.y = CP_Math_LerpFloat(boss->pos.y, _chargeTo.y, _chargeProgress);
	update_rect_hitbox(&boss->hitbox, boss->width, boss->pos, BOSS_HITBOX_OFFSET_X(boss->width), 0.f);

	// Damage player
	struct Rectangle towerHitbox = get_tower_hitbox(*tower);
	if (are_rects_intersecting(player->hitbox, boss->hitbox) && !_chargeHasAttackedPlayer) {
		_chargeHasAttackedPlayer = true;
		if (playerInstance.isReflectingDmg)
			boss_take_damage(boss, boss->attackPower);
		else
			player_take_damage(player, boss->attackPower);
	}
	if (are_rects_intersecting(player->hitbox, towerHitbox))
		tower_take_damage(tower, 1);

	// Done charging to pos
	// Reset values
	if (boss->pos.x == _chargeTo.x && boss->pos.y == _chargeTo.y) {
		boss->path.hasUpdated = false;
		boss->state = BOSS_STATE_MOVING_TO_TOWER;
		_chargeTo = CP_Vector_Zero();
		_chargeProgress = 0.f;
		_secAtkCooldown = 0.f;
		_secAtkDelay = 0.f;
		_chargeHasAttackedPlayer = false;
	}
}

// God bless YoshiP
void render_boss_one_sec_atk_telegraph(struct Boss boss, struct ChocoShooter player) {
	if (_chargeProgress != 0.f || boss.state != BOSS_STATE_ATTACKING_SECONDARY) return;

	CP_Settings_Fill(CP_Color_Create(255, 0, 0, 75));

	// Direction vector and angle to player
	float angle = atan2f(_chargeTo.y - boss.hitbox.pos.y, _chargeTo.x - boss.hitbox.pos.x);
	float distance = CP_Vector_Distance(boss.hitbox.pos, _chargeTo) + boss.hitbox.width / 2.f; // 'tis the telegraph length

	// To correctly render the rect from the middle point at its leftmost side
	// Updated to take into account for hitbox offset
	CP_Vector rectLeftMid = CP_Vector_Set(
		boss.hitbox.pos.x + cosf(angle) * (distance / 2.f),
		boss.hitbox.pos.y + sinf(angle) * (distance / 2.f));

	CP_Graphics_DrawRectAdvanced(rectLeftMid.x, rectLeftMid.y,
		distance,
		boss.hitbox.height,
		angle * (180.f / M_PI), 0.f);
}


int can_boss_one_sec_atk(struct Boss boss, struct ChocoShooter player) {
	// Min chargin g distance
	float minDistance = BOSS_ONE_SEC_ATK_DISTANCE + (fabsf(boss.hitbox.width) / 2.f) + (fabsf(player.hitbox.width) / 2.f);
	return ((CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance && _secAtkCooldown >= 10.f) ||
		boss.state == BOSS_STATE_ATTACKING_SECONDARY) && boss.state != BOSS_STATE_ATTACKING_PRIMARY;
}

bool boss_one_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
	if (_secAtkCooldown < 10.f)
		_secAtkCooldown += dt;

	if (boss->state != BOSS_STATE_MOVING_TO_TOWER &&
		boss->state != BOSS_STATE_MOVING_TO_PLAYER && 
		boss->state != BOSS_STATE_MOVING_TO_BARRIER) return;

	for (int i = 0; i < get_barrier_count(); i++) {
		if (are_rects_intersecting(boss->hitbox, barriers[i].hitbox)) {
			// barrier take attack
			if (!barrier_take_damage(i, barriers[i].maxHealth)) {
				boss->path.hasUpdated = false;
				return true;
			}
			break;
		}
	}

	struct Circle radar = { .pos = boss->pos, .diameter = 2.85f * boss->width };
	if (are_circle_rect_intersecting(radar, player.hitbox)) {
		if (boss->state != BOSS_STATE_MOVING_TO_PLAYER) {
			boss->path.hasUpdated = false;
			boss->state = BOSS_STATE_MOVING_TO_PLAYER;
		}
	}
	else {
		if (boss->state != BOSS_STATE_MOVING_TO_TOWER) {
			boss->path.hasUpdated = false;
			boss->state = BOSS_STATE_MOVING_TO_TOWER;
		}
	}
	_move_to_target(boss, hitbox, dt, grid, gridNoBarrier, nodeCount);
	return false;
}
#pragma endregion

#pragma region Boss 2
/*
void boss_two_pri_atk(struct Boss* boss, struct ChocoShooter* player, float dt) {
	// _priAtkCooldown in this func is used
	// for the boss recovering from attack (back to idle animation)

	boss->state = BOSS_STATE_ATTACKING_PRIMARY;
	const float attackingDelay = 3.1f,
		postAttackDelay = 1.9f;

	if (_meleeTwoVec.x == 0 && _meleeTwoVec.y == 0) {
		printf("Boss is performing melee attack\n");
		// Melee on the player pos
		_meleeTwoVec = player->pos;
	}

	// Add delay (attacking animation)
	if (_priAtkDelay < attackingDelay) {
		_priAtkDelay += dt * boss->attackSpeed;
		return;
	}

	// Damage player if conditions meet
	// In this case, it's the distance 100 (not 145)
	if (CP_Vector_Distance(_meleeTwoVec, player->hitbox.pos) <= 100)
		player_take_damage(player, boss->attackPower);
	printf("Boss has performed melee attack\n");

	// Post attack animation
	if (_priAtkCooldown < postAttackDelay)
	{
		_priAtkCooldown += dt;
		return;
	}

	printf("Boss has recovered from melee attack\n");
	boss->state = BOSS_STATE_MOVING_TO_TOWER;
	_priAtkDelay = _priAtkCooldown = 0.f;
	_meleeTwoVec = CP_Vector_Zero();
}

int can_boss_two_pri_atk(struct Boss boss, struct ChocoShooter player) {
	float minDistance = 100 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
	return CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance
		|| boss.state == BOSS_STATE_ATTACKING_PRIMARY;
}

void boss_two_sec_atk(struct Boss* boss, struct ChocoShooter* player, float dt) {
	boss->state = BOSS_STATE_ATTACKING_SECONDARY;
	const float attackingDelay = 1.2f,
		postAttackDelay = 4.5f;
	printf("Boss is performing slam attack\n");

	// Add delay (animation)
	if (_secAtkDelay < attackingDelay) {
		_secAtkDelay += dt;
		return;
	}

	if (_previousPlayerSpeed <= 0.f)
		_previousPlayerSpeed = player->speed;

	player->speed /= 1.5f;

	printf("Slam attack\n");

	// Post attack animation
	if (_secAtkCooldown < postAttackDelay)
	{
		_secAtkCooldown += dt;
		return;
	}

	// Reset values
	if (boss->pos.x == _chargeTo.x && boss->pos.y == _chargeTo.y) {
		boss->state = BOSS_STATE_MOVING_TO_TOWER;
		_secAtkCooldown = 0.f;
		_secAtkDelay = 0.f;
		player->speed = _previousPlayerSpeed;
		_previousPlayerSpeed = 0.f;
	}
}

int can_boss_two_sec_atk(struct Boss boss, struct ChocoShooter player) {
	return _secAtkCooldown >= 45.f || boss.state == BOSS_STATE_ATTACKING_SECONDARY;
}

void boss_two_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
	if (_secAtkCooldown < 45.f)
		_secAtkCooldown += dt;

	if (boss->state != BOSS_STATE_MOVING_TO_TOWER && boss->state != BOSS_STATE_MOVING_TO_PLAYER) return;

	struct Circle radar = { .pos = boss->pos, .diameter = 1.35f * boss->width };
	if (are_circle_rect_intersecting(radar, player.hitbox))
		boss->state = BOSS_STATE_MOVING_TO_PLAYER;

	_move_to_target(boss, hitbox, dt, grid, gridNoBarrier, nodeCount);
}
*/
#pragma endregion

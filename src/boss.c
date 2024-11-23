#pragma
#include "cprocessing.h"
#include "boss.h"
#include "player.h"
#include "utils.h"
#include "cprocessing.h"
#include "hittest.h"
#include "astar.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

// ************* Private variables ************* //
//         _xAtkDelay is for pre-damage          //
//      _xAttackCooldown is for post-damage      //
//       Unless specified in the function        //
// ********************************************* //
static float _priAtkDelay = 0.f, _priAtkCooldown = 0.f;
static float _secAtkDelay = 0.f, _secAtkCooldown = 0.f;

// Boss one attack private variables
static float _chargeProgress;
static CP_Vector _meleeOneVec, _chargeTo;

// Boss two attack private variables
static CP_Vector _meleeTwoVec;
static float _previousPlayerSpeed = 0.f;

void dispose_boss(struct Boss* boss) {
	if (boss == NULL) return;
	boss->hitbox.pos = boss->pos = CP_Vector_Set(-500.f, -500.f);
	CP_Image_Free(boss->img);
	/*free(boss->move);
	free(boss->primaryAttack);
	free(boss->canPrimaryAttack);
	free(boss->canSecondaryAttack);
	free(boss->secondaryAttack);
	free(boss->img);

	boss->move = NULL;
	boss->primaryAttack = NULL;
	boss->canPrimaryAttack = NULL;
	boss->canSecondaryAttack = NULL;
	boss->secondaryAttack = NULL;
	boss->img = NULL;
	free(boss);
	boss = NULL;*/
}

static void _move_to_target(struct Boss* boss, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
	int pathArray[50];
	int pathNodeCount = 0;
	int startNodeIndex = get_nearest_node_index(grid, nodeCount, boss->pos);
	int endNodeIndex = get_nearest_node_index(grid, nodeCount, hitbox.pos);
	if (startNodeIndex == endNodeIndex) {
		boss->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(hitbox.pos, boss->pos)));
	}
	else {
		pathNodeCount = find_astar_path(grid, nodeCount, startNodeIndex, endNodeIndex, pathArray);

		if (pathNodeCount) {
			boss->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(grid[pathArray[1]].pos, boss->pos)));
		}
		else {
			int startNodeIndexNB = get_nearest_node_index(gridNoBarrier, nodeCount, boss->pos);
			int endNodeIndexNB = get_nearest_node_index(gridNoBarrier, nodeCount, hitbox.pos);
			find_astar_path(gridNoBarrier, nodeCount, startNodeIndexNB, endNodeIndexNB, pathArray);
			boss->moveDirection = CP_Vector_Normalize((CP_Vector_Subtract(grid[pathArray[1]].pos, boss->pos)));
		}
	}
	boss->width = (boss->moveDirection.x < 0.f && boss->width > 0.f) || (boss->moveDirection.x > 0.f && boss->width < 0.f) ? -boss->width : boss->width;
	boss->pos.x += boss->moveDirection.x * boss->speed * dt;
	boss->pos.y += boss->moveDirection.y * boss->speed * dt;
	update_rect_hitbox(&boss->hitbox, 0.f, 0.f, boss->pos);
}

struct Boss init_boss_one(float width, float speed, int health, int attack_power, float attack_speed) {
	CP_Image img = CP_Image_Load("Assets/boss_lvl1.png");
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
		.primaryAttack = boss_one_pri_atk,
		.canPrimaryAttack = can_boss_one_pri_atk,
		.secondaryAttack = boss_one_sec_atk,
		.canSecondaryAttack = can_boss_one_sec_atk,
		.move = boss_one_move
	};

	return boss;
}

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

void render_boss(struct Boss boss) {
	CP_Image_Draw(boss.img, boss.pos.x, boss.pos.y, boss.width, boss.height, 255);
}

void spawn_boss(struct Boss* boss, float x, float y) {
	CP_Vector vec = CP_Vector_Set(x, y);
	boss->pos = vec;
	boss->state = BOSS_STATE_MOVING_TO_TOWER;
	update_rect_hitbox(&boss->hitbox, 0.f, 0.f, vec);
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
void boss_one_pri_atk(struct Boss *boss, struct ChocoShooter* player, float dt) {
	// _priAtkCooldown in this func is used
	// for the boss recovering from attack (back to idle animation)

	boss->state = BOSS_STATE_ATTACKING_PRIMARY;
	const float attackingDelay = 2.7f,
		postAttackDelay = 1.5f;

	if (_meleeOneVec.x == 0 && _meleeOneVec.y == 0) {
		printf("Boss is performing melee attack\n");
		// Melee on the player pos
		_meleeOneVec = player->pos;
	}

	// Add delay (attacking animation)
	if (_priAtkDelay < attackingDelay) {
		_priAtkDelay += dt * boss->attackSpeed;
		return;
	}

	// Damage player if conditions meet
	// In this case, it's the distance 80 (not 125)
	if (CP_Vector_Distance(_meleeOneVec, player->hitbox.pos) <= 80)
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
	_meleeOneVec = CP_Vector_Zero();
}

int can_boss_one_pri_atk(struct Boss boss, struct ChocoShooter player) {
	// 75 is min range to snapshot the player's pos for the attack,
	// then add half width (distance from center origin of both entities)
	// You want to the boss to attack as soon as the edge of its hitbox is as close
	// to the edge of the player's hitbox (rather than the center origin)
	float minDistance = 75 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
	return (CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance
		|| boss.state == BOSS_STATE_ATTACKING_PRIMARY);
}

void boss_one_sec_atk(struct Boss *boss, struct ChocoShooter* player, float dt) {
	boss->state = BOSS_STATE_ATTACKING_SECONDARY;
	const float attackingDelay = 3.5f;
	printf("Boss is performing charge attack\n");

	// Set its charge to pos to player's
	if (_chargeTo.x == 0 && _chargeTo.y == 0)
		_chargeTo = player->pos;

	// Add delay (attacking animation)
	if (_secAtkDelay < attackingDelay) {
		_secAtkDelay += dt * boss->attackSpeed;
		return;
	}

	printf("Charge attack\n");

	// Lerp
	// Use speed instead
	_chargeProgress += (boss->speed / 3.f) * dt;
	boss->pos.x = CP_Math_LerpFloat(boss->pos.x, _chargeTo.x, _chargeProgress);
	boss->pos.y = CP_Math_LerpFloat(boss->pos.y, _chargeTo.y, _chargeProgress);

	// Damage player
	// 105 is the max distance to get hit by
	if (CP_Vector_Distance(boss->pos, player->pos) + (boss->hitbox.width / 2) + (player->hitbox.width / 2)
		<= 105)
		player_take_damage(player, boss->attackPower);

	// Done charging to pos
	// Reset values
	if (boss->pos.x == _chargeTo.x && boss->pos.y == _chargeTo.y) {
		boss->state = BOSS_STATE_MOVING_TO_TOWER;
		_chargeTo = CP_Vector_Zero();
		_chargeProgress = 0.f;
		_secAtkCooldown = 0.f;
		_secAtkDelay = 0.f;
	}
}

int can_boss_one_sec_atk(struct Boss boss, struct ChocoShooter player) {
	// 175 is for charging distance
	float minDistance = 175 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
	return (CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance && _secAtkCooldown >= 15.f) ||
		boss.state == BOSS_STATE_ATTACKING_SECONDARY;
}

void boss_one_move(struct Boss* boss, struct ChocoShooter player, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount) {
	if (_secAtkCooldown < 15.f)
		_secAtkCooldown += dt;

	if (boss->state != BOSS_STATE_MOVING_TO_TOWER && boss->state != BOSS_STATE_MOVING_TO_PLAYER) return;

	struct Circle radar = { .pos = boss->pos, .diameter = 1.35f * boss->width };
	if (are_circle_rect_intersecting(radar, player.hitbox))
		boss->state = BOSS_STATE_MOVING_TO_PLAYER;

	_move_to_target(boss, hitbox, dt, grid, gridNoBarrier ,nodeCount);
}
#pragma endregion

#pragma region Boss 2
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
#pragma endregion

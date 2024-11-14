#pragma
#include "cprocessing.h"
#include "flicker.h"
#include "boss.h"
#include "player.h"
#include "utils.h"
#include "cprocessing.h"
#include "hittest.h"
#include <stdio.h>
#include <stdbool.h>

// ************* Private variables ************* //
//         _xAtkDelay is for pre-damage          //
//      _xAttackCooldown is for post-damage      //
//       Unless specified in the function        //
// ********************************************* //
static float _priAtkDelay = 0.f, _priAtkCooldown = 0.f;
static float _secAtkDelay = 0.f, _secAtkCooldown = 0.f;
static int _isInPriAtk = 0, _isInSecAtk = 0;
// Boss one secondary attack private variable
static float _chargeProgress;
CP_Vector _chargeTo;

static void _move_boss(struct Boss* boss, CP_Vector move_to, float dt) {
	CP_Vector moveDirection = {
		// Get whole number instead
		// So it's either -1, 0, or 1
		.x = CP_Math_ClampFloat((int)move_to.x - (int)boss->pos.x, -1.f, 1.f),
		.y = CP_Math_ClampFloat((int)move_to.y - (int)boss->pos.y, -1.f, 1.f)
	};

	// Movement state
	boss->movement = STOPPED;
	if (moveDirection.x > 0)
		boss->movement |= MOVING_RIGHT; // This will add MOVING_RIGHT to movement
	if (moveDirection.x < 0)
		boss->movement |= MOVING_LEFT;
	if (moveDirection.y > 0)
		boss->movement |= MOVING_DOWN;
	if (moveDirection.y < 0)
		boss->movement |= MOVING_UP;

	// Check if boss is facing right
	// Flip sprite depending on direction
	if (boss->movement & MOVING_RIGHT)
		boss->width = boss->width > 0 ? -boss->width : boss->width;
	else
		boss->width = boss->width < 0 ? -boss->width : boss->width;

	// Normalize
	if (moveDirection.x != 0 || moveDirection.y != 0)
		moveDirection = CP_Vector_Normalize(moveDirection);

	boss->pos.x += moveDirection.x * boss->speed * dt;
	boss->pos.y += moveDirection.y * boss->speed * dt;

	update_rect_hitbox(&boss->hitbox, 0.f, 0.f, boss->pos);
}

struct Boss init_boss_one(float width, float speed, int health, int attack_power, float attack_speed) {
	CP_Image img = CP_Image_Load("Assets/boss_lvl1.png");
	float preservedHeight = get_preserved_heightf(CP_Image_GetWidth(img), CP_Image_GetHeight(img), width);
	struct Boss boss = {
		.img = img,
		.speed = speed,
		.health = boss.maxHealth = health,
		.hitbox.pos = boss.pos = CP_Vector_Set(-550.f, 50.f),
		.width = width,
		.height = preservedHeight,
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

void render_boss(struct Boss boss, struct Camera cam) {
	CP_Vector vec = get_cam_pos_rel(boss.pos, cam);
	CP_Image_Draw(boss.img, vec.x, vec.y, boss.width, boss.height, 255);
}

void boss_take_damage(struct Boss* boss, int damage) {
	boss->health = CP_Math_ClampInt(boss->health - damage, 0, boss->maxHealth);

	if (boss->health > 0) {
		printf("Boss took %d damage, health is now %d\n", damage, boss->health);
		return;
	}

	printf("Boss has been defeated!\n");
}

#pragma region Boss 1
void boss_one_pri_atk(struct Boss *boss, struct ChocoShooter* player, float dt) {
	// _priAtkCooldown in this func is used
	// for the boss recovering from attack (back to idle animation)

	_isInPriAtk = 1;
	const float attackingDelay = 2.7f,
		postAttackDelay = 1.5f;
	printf("Boss is performing melee attack\n");

	// Add delay (attacking animation)
	if (_priAtkDelay < attackingDelay) {
		_priAtkDelay += dt;
		return;
	}

	// Damage player if conditions meet
	// In this case, it's the distance
	if (_can_boss_one_pri_atk_dist(*boss, *player))
		player_take_damage(player, boss->attackPower);
	printf("Boss has performed melee attack\n");

	// Post attack animation
	if (_priAtkCooldown < postAttackDelay)
	{
		_priAtkCooldown += dt;
		return;
	}

	printf("Boss has recovered from melee attack\n");
	_isInPriAtk = 0;
	_priAtkDelay = _priAtkCooldown = 0.f;
}

int can_boss_one_pri_atk(struct Boss boss, struct ChocoShooter player) {
	return (_can_boss_one_pri_atk_dist(boss, player) || _isInPriAtk) && !_isInSecAtk;
}

static int _can_boss_one_pri_atk_dist(struct Boss boss, struct ChocoShooter player) {
	// 125 is attack range, then add half width (distance from center origin of both entities)
	// You want to the boss to attack as soon as the edge of its hitbox is as close
	// to the edge of the player's hitbox (rather than the center origin)
	float minDistance = 125 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
	return CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance;
}

void boss_one_sec_atk(struct Boss *boss, struct ChocoShooter* player, float dt) {
	_isInSecAtk = 1;
	const float attackingDelay = 3.5f;
	printf("Boss is performing charge attack\n");

	// Set its charge to pos to player's
	if (_chargeTo.x == 0 && _chargeTo.y == 0)
		_chargeTo = player->pos;

	// Add delay (attacking animation)
	if (_secAtkDelay < attackingDelay) {
		_secAtkDelay += dt;
		return;
	}

	printf("Charge attack\n");

	// Lerp
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
		_isInSecAtk = 0;
		_chargeTo = CP_Vector_Zero();
		_chargeProgress = 0.f;
		_secAtkCooldown = 0.f;
		_secAtkDelay = 0.f;
	}
}

int can_boss_one_sec_atk(struct Boss boss, struct ChocoShooter player) {
	// 275 is for charging distance
	float minDistance = 275 + (boss.hitbox.width / 2) + (player.hitbox.width / 2);
	return (CP_Vector_Distance(boss.hitbox.pos, player.hitbox.pos) <= minDistance && _secAtkCooldown >= 15.f) ||
		_isInSecAtk && !_isInPriAtk;
}

void boss_one_move(struct Boss* boss, CP_Vector move_to, float dt) {
	if (_secAtkCooldown < 15.f)
		_secAtkCooldown += dt;

	// Boss is in attack state, don't move
	if (_isInPriAtk || _isInSecAtk) return;

	_move_boss(boss, move_to, dt);
}
#pragma endregion

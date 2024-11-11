#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"
#include "player.h"
#include "tower.h"

#define ENEMY_HITBOX_OFFSET_X(enemyWidth) (- 1 / 33.f * enemyWidth)
#define ENEMY_HITBOX_OFFSET_Y(enemyHeight) (- 8.f / 77.f * enemyHeight)

enum EnemyState {
	STATE_MOVING,
	STATE_CHASE_PLAYER,
	STATE_DETOUR,
	STATE_ATTACKING
};

struct ChocoRoach {
	CP_Image img;
	CP_Vector pos;
	float width;
	float height;
	float speed;
	int health;
	int maxHealth;
	int dmg;
	enum Movement movement;
	enum EnemyState currentState;
	struct Rectangle hitbox;
};

struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float speed, int maxHealth, int dmg);
void render_enemy(struct ChocoRoach roach, struct Camera cam);
void update_pos_enemy(struct ChocoRoach *roach, float dt);
void enemy_chase_player(struct ChocoRoach* roach, struct ChocoShooter player);
void enemy_move_to_tower(struct ChocoRoach* roach, struct DefenseTarget tower);
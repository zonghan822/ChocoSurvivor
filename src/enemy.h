#pragma once
#include <stdbool.h>
#include "cprocessing.h"
#include "utils.h"
#include "player.h"
#include "tower.h"
#include "astar.h"

#define MAX_ENEMY_COUNT 100
#define ENEMY_WIDTH 60.f
#define ENEMY_HITBOX_OFFSET_X(enemyWidth) (- 1 / 33.f * enemyWidth)
#define ENEMY_HITBOX_OFFSET_Y(enemyHeight) (- 8.f / 77.f * enemyHeight)
#define ENEMY_HEALTHBAR_DISPLAY_TIME 3.f

enum EnemyState {
	ENEMY_STATE_DIED = 0,
	ENEMY_STATE_MOVING_TO_TOWER,
	ENEMY_STATE_MOVING_TO_PLAYER,
	ENEMY_STATE_DASHING,
	ENEMY_STATE_ATTACKING,
	ENEMY_STATE_IDLE
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
	CP_Vector moveDirection;
	enum EnemyState currentState;
	struct Rectangle hitbox;
	BOOL isTakingDamage; // for healthbar rendering
	float healthBarDisplayTime;
	int barrierAttack;
};

// Takes in single enemy
struct ChocoRoach init_enemy(float spawnMinY, float spawnMaxY, float spawnWidth, float speed, int maxHealth, int dmg);
void enemy_move_to_target(struct ChocoRoach* roach, struct Rectangle hitbox, float dt, PathNode* grid, PathNode* gridNoBarrier, int nodeCount);
void render_enemy_health_bar(struct ChocoRoach* roach, struct Camera cam);
int enemy_take_damage(struct ChocoRoach* roach, int dmg);

// Takes in array of enemies
void render_enemies(struct ChocoRoach* roachesArr, struct Camera cam, int size);
void sort_enemies(struct ChocoRoach* roachesArr, int size);

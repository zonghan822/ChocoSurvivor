#pragma once
#include "cprocessing.h"
#include "utils.h"
#include "player.h"

enum EnemyState {
	STATE_MOVING,
	STATE_ATTACKING
};

struct ChocoRoach {
	CP_Image img;
	CP_Vector pos;
	float width;
	float height;
	float speed;
	int health;
	int dmg;
	enum Movement movement;
	enum EnemyState current_state;
};


void render_enemy(struct ChocoRoach roach);
void update_pos_enemy(struct ChocoRoach *roach, float dt);
void chase_player(struct ChocoRoach* roach, struct ChocoShooter player);
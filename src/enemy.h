#pragma once
#include "cprocessing.h"
#include "utils.h"

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

void render_enemies(struct ChocoRoach roaches[], int count);
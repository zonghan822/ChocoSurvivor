#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "cprocessing.h"
#include "mainmenu.h"

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define NULL ((void*) 0)
#define MAX_ENEMY_COUNT 100

// return random double-precision floating-point values in range [m, n]
float rand_float(float m, float n) {
	return ((float)rand() / (float)RAND_MAX) * (n - m) + m;
}

enum Movement {
	STOP,
	MOVING_RIGHT,
	MOVING_DOWN,
	MOVING_LEFT,
	MOVING_UP,
	MOVING_UP_LEFT,
	MOVING_UP_RIGHT,
	MOVING_DOWN_LEFT,
	MOVING_DOWN_RIGHT
};

enum EnemyState {
	STATE_MOVING,
	STATE_ATTACKING
};

struct ChocoRoach {
	CP_Image img;
	CP_Vector pos;
	float radius;
	float degree;
	float speed;
	int health;
	int dmg;
	enum Movement movement;
	enum EnemyState current_state;
};

struct ChocoRoach roachesArr[MAX_ENEMY_COUNT];
float windowWidth, windowHeight;
int roachesCnt;

void game_init(void)
{
	// Get window width and height
	windowWidth = CP_System_GetWindowWidth();
	windowHeight = CP_System_GetWindowHeight();

	// Define enemy spawn area
	float spawnMinY = windowHeight / 10;
	float spawnMaxY = windowHeight * 9 / 10;

	// Define enemy count
	roachesCnt = 4;

	srand(time(NULL));
	for (int i = 0; i < roachesCnt; i++) {
		
		// Define properties of enemy
		struct ChocoRoach chocoRoach = {
			.img = NULL,
			.pos.x = -35.f,
			.pos.y = rand_float(spawnMinY, spawnMaxY), // Generate random y position for enemies
			.radius = 35.f,
			.degree = 0.f,
			.speed = 350.f,
			.health = 100,
			.dmg = 10,
			.movement = MOVING_RIGHT,
			.current_state = STATE_MOVING
		};

		// Store enemies in array
		roachesArr[i] = chocoRoach;
	}

}

void game_update(void)
{
	// Clear background with grey background colour
	CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

	float dtime = CP_System_GetDt();

	for (int i = 0; i < roachesCnt; i++) {

		// Draw agent
		float x = roachesArr[i].pos.x;
		float y = roachesArr[i].pos.y;
		float r = roachesArr[i].radius;
		float d = roachesArr[i].degree;

		// Draw circle
		CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
		CP_Graphics_DrawCircle(x, y, r * 2.0f);

		// Draw triangle 
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Graphics_DrawTriangleAdvanced(x + r, y, x - r * cos(0.9), y + r * sin(0.9), x - r * cos(0.9), y - r * sin(0.9), d);
	}


	// Enemies movement logic
	for (int i = 0; i < roachesCnt; i++) {

		// Update position of player and agents
		switch (roachesArr[i].movement) {
		case MOVING_UP:
			roachesArr[i].pos.y -= (roachesArr[i].speed * dtime);
			roachesArr[i].degree = 270.0f;
			break;
		case MOVING_DOWN:
			roachesArr[i].pos.y += (roachesArr[i].speed * dtime);
			roachesArr[i].degree = 90.0f;
			break;
		case MOVING_LEFT:
			roachesArr[i].pos.x -= (roachesArr[i].speed * dtime);
			roachesArr[i].degree = 180.0f;
			break;
		case MOVING_RIGHT:
			roachesArr[i].pos.x += (roachesArr[i].speed * dtime);
			roachesArr[i].degree = 0.0f;
			break;
		}
	}

	// Exit to Main Menu
	/*if (CP_Input_KeyTriggered(KEY_Q)) {
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	}*/
}

void game_exit(void)
{

}
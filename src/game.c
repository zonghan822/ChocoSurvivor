#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
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

// Direction enum, player, and boss structs
enum Direction { UP, DOWN, LEFT, RIGHT };

struct boss1 {
    CP_Vector position;
    float width;
    float height;
    float speed;
    CP_Image image;
    char imageTag[50];
    int maxhealth;
    bool isFlipped; // Track if the boss should be facing left
};

// Initialize boss instance
struct boss1 boss1instance = {
    .height = 500.0f,
    .width = 500.0f,
    .speed = 80.0f,
    .maxhealth = 40,
    .position = {100.0f, 450.0f},
    .imageTag = "boss_lvl1.png",
    .isFlipped = false
};

struct chocoshooter {
    CP_Vector position;
    float width;
    float height;
    float speed;
    CP_Image image;
    char imageTag[50];
    bool isFlipped;
    enum Direction lastDirection;
    int health;
    int maxhealth;
};

// Initialize player instance
struct chocoshooter chocoShooterInstance = {
    .position = {100.0f, 100.0f},
    .width = 200.0f,
    .height = 300.0f,
    .speed = 200.0f,
    .health = 3,
    .maxhealth = 3,
    .imageTag = "choco shooter.png",
    .isFlipped = false,
    .lastDirection = RIGHT
};

void takeDamage(struct chocoshooter* player, int damage) {
    if (player->health > 0) {
        player->health -= damage;
        if (player->health < 0) {
            player->health = 0;  // Ensure health doesn't go below zero
            printf("Choco Shooter has been defeated!\n");
            // You can add code here to handle game-over or respawn logic
        }
        else {
            printf("Choco Shooter took %d damage, health is now %d\n", damage, player->health);
        }
    }
}

struct ChocoRoach roachesArr[MAX_ENEMY_COUNT];
float windowWidth, windowHeight;
int roachesCnt;

void game_init(void)
{
// ZH:
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

// DK:
    chocoShooterInstance.image = CP_Image_Load("Assets/choco_shooter.png");
    boss1instance.image = CP_Image_Load("Assets/boss_lvl1.png");
}


void game_update(void)
{
	// Clear background with grey background colour
	CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

	float dt = CP_System_GetDt();

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
			roachesArr[i].pos.y -= (roachesArr[i].speed * dt);
			roachesArr[i].degree = 270.0f;
			break;
		case MOVING_DOWN:
			roachesArr[i].pos.y += (roachesArr[i].speed * dt);
			roachesArr[i].degree = 90.0f;
			break;
		case MOVING_LEFT:
			roachesArr[i].pos.x -= (roachesArr[i].speed * dt);
			roachesArr[i].degree = 180.0f;
			break;
		case MOVING_RIGHT:
			roachesArr[i].pos.x += (roachesArr[i].speed * dt);
			roachesArr[i].degree = 0.0f;
			break;
		}
	}

    // Player movement and setting lastDirection
    if (CP_Input_KeyDown(KEY_W)) {
        chocoShooterInstance.position.y -= chocoShooterInstance.speed * dt;
        chocoShooterInstance.lastDirection = UP;
    }
    if (CP_Input_KeyDown(KEY_S)) {
        chocoShooterInstance.position.y += chocoShooterInstance.speed * dt;
        chocoShooterInstance.lastDirection = DOWN;
    }
    if (CP_Input_KeyDown(KEY_A)) {
        chocoShooterInstance.position.x -= chocoShooterInstance.speed * dt;
        chocoShooterInstance.isFlipped = true;
        chocoShooterInstance.lastDirection = LEFT;
    }
    if (CP_Input_KeyDown(KEY_D)) {
        chocoShooterInstance.position.x += chocoShooterInstance.speed * dt;
        chocoShooterInstance.isFlipped = false;
        chocoShooterInstance.lastDirection = RIGHT;
    }

    // Dash movement with spacebar
    if (CP_Input_KeyTriggered(KEY_SPACE)) {
        float dashDistance = 100.0f;
        switch (chocoShooterInstance.lastDirection) {
        case UP:
            chocoShooterInstance.position.y -= dashDistance;
            break;
        case DOWN:
            chocoShooterInstance.position.y += dashDistance;
            break;
        case LEFT:
            chocoShooterInstance.position.x -= dashDistance;
            break;
        case RIGHT:
            chocoShooterInstance.position.x += dashDistance;
            break;
        }
    }

    if (CP_Input_KeyTriggered(KEY_H)) {  // Press 'H' to take 1 damage
        takeDamage(&chocoShooterInstance, 1);
    }
    char healthDisplay[256];
    sprintf_s(healthDisplay, sizeof(healthDisplay), "Health: %d", chocoShooterInstance.health);
    CP_Settings_TextSize(30);
    CP_Font_DrawText(healthDisplay, 50, 50);

    // Boss follows player and changes direction
    CP_Vector directionToPlayer;
    directionToPlayer.x = chocoShooterInstance.position.x - boss1instance.position.x;
    directionToPlayer.y = chocoShooterInstance.position.y - boss1instance.position.y;

    // Check horizontal direction to flip boss's image
    if (directionToPlayer.x < 0) {
        boss1instance.isFlipped = true;  // Face left if player is to the left
    }
    else {
        boss1instance.isFlipped = false; // Face right if player is to the right
    }

    // Calculate distance and normalize direction vector
    float distance = CP_Vector_Length(directionToPlayer);
    if (distance != 0) {
        directionToPlayer = CP_Vector_Scale(directionToPlayer, boss1instance.speed * dt / distance);
    }

    // Update boss position toward the player
    boss1instance.position.x += directionToPlayer.x;
    boss1instance.position.y += directionToPlayer.y;

    // Draw player
    float drawWidth = chocoShooterInstance.isFlipped ? -chocoShooterInstance.width : chocoShooterInstance.width;
    CP_Image_Draw(
        chocoShooterInstance.image,
        chocoShooterInstance.position.x,
        chocoShooterInstance.position.y,
        drawWidth,
        chocoShooterInstance.height,
        255
    );

    // Draw boss with flipping
    float bossDrawWidth = boss1instance.isFlipped ? -boss1instance.width : boss1instance.width;
    if (levels() == 1) {
        CP_Image_Draw(
            boss1instance.image,
            boss1instance.position.x,
            boss1instance.position.y,
            bossDrawWidth,
            boss1instance.height,
            255
        );
    }

	// Exit to Main Menu
	if (CP_Input_KeyTriggered(KEY_Q)) {
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	}
}

void game_exit(void)
{
    CP_Image_Free(&chocoShooterInstance.image);
    CP_Image_Free(&boss1instance.image);
}


int levels(void) {
    int lvl = 1;
    int enemies = 5;
    if (lvl == 1 && enemies == 0) {
        return 1;
    }
    else if (lvl == 1 && enemies != 0) {
        return 0;
    }
    return 0;
}




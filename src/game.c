#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "cprocessing.h"
#include "utils.h"
#include "mainmenu.h"
#include "player.h"
#include "enemy.h"
#include "boss.h"
#include "flicker.h"

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
//#define NULL ((void*)0)
#define MAX_ENEMY_COUNT 100

struct ChocoRoach roaches[MAX_ENEMY_COUNT];
struct Boss boss1instance;
struct ChocoShooter chocoShooterInstance;

int windowWidth, windowHeight, roachesCnt;
bool isDashing = false;
float dashProgress, lastDash;
CP_Vector dashTo;
const float DASH_COOLDOWN = 2.0, DASH_DISTANCE = 150.0, DASH_SPEED = 10.0;
CP_Image gameOverImage;
CP_Image chococursor;



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

    // Initialize dash variables
    dashProgress = 0.0;
    lastDash = 2.1;

	srand(time(NULL));
	for (int i = 0; i < roachesCnt; i++) {
		
		// Define properties of enemy
		struct ChocoRoach chocoRoach = {
			.img = NULL,
			.pos.x = -35.f,
			.pos.y = rand_float(spawnMinY, spawnMaxY), // Generate random y pos for enemies
			.radius = 35.f,
			.degree = 0.f,
			.speed = 350.f,
			.health = 100,
			.dmg = 10,
			.movement = MOVING_RIGHT,
			.current_state = STATE_MOVING
		};

		// Store enemies in array
		roaches[i] = chocoRoach;
	}
    chococursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);

    // Initialize boss instance
    boss1instance.height = 500.0f;
    boss1instance.width = 500.0f;
    boss1instance.speed = 80.0f;
    boss1instance.maxHealth = 20;
    boss1instance.pos.x = 100.0f;
    boss1instance.pos.y = 450.0f;
    boss1instance.attackPower = 5;
    boss1instance.img = CP_Image_Load("Assets/boss_lvl1.png");
    boss1instance.isFlipped = false;

    // Initialize player instance
    chocoShooterInstance.pos.x = 100.0f;
    chocoShooterInstance.pos.y = 100.0f;
    chocoShooterInstance.width = 200.0f,
        chocoShooterInstance.height = 300.0f;
        chocoShooterInstance.speed = 200.0f;
        chocoShooterInstance.health = 20;
        chocoShooterInstance.shield = 1;
        chocoShooterInstance.attackPower = 5;
    chocoShooterInstance.attackSpeed = 5;
    chocoShooterInstance.img = CP_Image_Load("Assets/choco_shooter.png");
    chocoShooterInstance.isFlipped = false,
    chocoShooterInstance.movement = MOVING_RIGHT;

    gameOverImage = CP_Image_Load("Assets/gg.png");

    //set initial flickering states
    playerFlickerState.isVisible = true;
    bossFlickerState.isVisible = true;
}


void game_update(void)
{
	// Clear background with grey background colour
	CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

	float dt = CP_System_GetDt();
    render_enemies(roaches, roachesCnt);

	// Enemies movement logic
	for (int i = 0; i < roachesCnt; i++) {

		// Update position of enemies
		switch (roaches[i].movement) {
		case MOVING_UP:
			roaches[i].pos.y -= (roaches[i].speed * dt);
			roaches[i].degree = 270.0f;
			break;
		case MOVING_DOWN:
			roaches[i].pos.y += (roaches[i].speed * dt);
			roaches[i].degree = 90.0f;
			break;
		case MOVING_LEFT:
			roaches[i].pos.x -= (roaches[i].speed * dt);
			roaches[i].degree = 180.0f;
			break;
		case MOVING_RIGHT:
			roaches[i].pos.x += (roaches[i].speed * dt);
			roaches[i].degree = 0.0f;
			break;
		}
	}

    // Player movement and setting movement
    chocoShooterInstance.movement = STOPPED;
    if (CP_Input_KeyDown(KEY_W))
        chocoShooterInstance.movement |= MOVING_UP;
    if (CP_Input_KeyDown(KEY_S))
        chocoShooterInstance.movement |= MOVING_DOWN;
    if (CP_Input_KeyDown(KEY_A))
        chocoShooterInstance.movement |= MOVING_LEFT;
    if (CP_Input_KeyDown(KEY_D))
        chocoShooterInstance.movement |= MOVING_RIGHT;

    // W+S and A+D inputs should stop the character
    // Clear both UP and DOWN bits
    // Clear both LEFT and RIGHT bits
    if ((chocoShooterInstance.movement & MOVING_UP) && (chocoShooterInstance.movement & MOVING_DOWN))
        chocoShooterInstance.movement &= ~(MOVING_UP | MOVING_DOWN);
    if ((chocoShooterInstance.movement & MOVING_LEFT) && (chocoShooterInstance.movement & MOVING_RIGHT))
        chocoShooterInstance.movement &= ~(MOVING_LEFT | MOVING_RIGHT);

    // So character's movement isn't "boosted" when mooving diagonally
    float diagonalSpeed = chocoShooterInstance.speed / sqrt(2.0f);

    switch (chocoShooterInstance.movement)
    {
        case MOVING_UP:
            chocoShooterInstance.pos.y -= chocoShooterInstance.speed * dt;
            break;
        case MOVING_RIGHT:
            chocoShooterInstance.pos.x += chocoShooterInstance.speed * dt;
            break;
        case MOVING_DOWN:
            chocoShooterInstance.pos.y += chocoShooterInstance.speed * dt;
            break;
        case MOVING_LEFT:
            chocoShooterInstance.pos.x -= chocoShooterInstance.speed * dt;
            break;
        case MOVING_UP | MOVING_RIGHT:
            chocoShooterInstance.pos.x += diagonalSpeed * dt;
            chocoShooterInstance.pos.y -= diagonalSpeed * dt;
            break;
        case MOVING_UP | MOVING_LEFT:
            chocoShooterInstance.pos.x -= diagonalSpeed * dt;
            chocoShooterInstance.pos.y -= diagonalSpeed * dt;
            break;
        case MOVING_DOWN | MOVING_RIGHT:
            chocoShooterInstance.pos.x += diagonalSpeed * dt;
            chocoShooterInstance.pos.y += diagonalSpeed * dt;
            break;
        case MOVING_DOWN | MOVING_LEFT:
            chocoShooterInstance.pos.x -= diagonalSpeed * dt;
            chocoShooterInstance.pos.y += diagonalSpeed * dt;
    }

    if (lastDash < DASH_COOLDOWN)
        lastDash += dt;

    // Dash movement with spacebar
    if (CP_Input_KeyTriggered(KEY_SPACE) && !isDashing && lastDash >= DASH_COOLDOWN)
    {
        dashTo = chocoShooterInstance.pos;
        float diagonalDash = DASH_DISTANCE / sqrt(2.0);

        switch (chocoShooterInstance.movement) {
        case MOVING_UP:
            dashTo.y -= DASH_DISTANCE;
            break;
        case MOVING_DOWN:
            dashTo.y += DASH_DISTANCE;
            break;
        case MOVING_LEFT:
            dashTo.x -= DASH_DISTANCE;
            break;
        case MOVING_RIGHT:
            dashTo.x += DASH_DISTANCE;
            break;
        case MOVING_UP | MOVING_LEFT:
            dashTo.y -= diagonalDash;
            dashTo.x -= diagonalDash;
            break;
        case MOVING_UP | MOVING_RIGHT:
            dashTo.x += diagonalDash;
            dashTo.y -= diagonalDash;
            break;
        case MOVING_DOWN | MOVING_LEFT:
            dashTo.x -= diagonalDash;
            dashTo.y += diagonalDash;
            break;
        case MOVING_DOWN | MOVING_RIGHT:
            dashTo.x += diagonalDash;
            dashTo.y += diagonalDash;
            break;
        }

        isDashing = TRUE;
        dashProgress = 0.0;
        lastDash = 0.0;
    }

    if (isDashing) 
    {
        dashProgress += DASH_SPEED * dt;

        chocoShooterInstance.pos.x = CP_Math_LerpFloat(chocoShooterInstance.pos.x, dashTo.x, dashProgress);
        chocoShooterInstance.pos.y = CP_Math_LerpFloat(chocoShooterInstance.pos.y, dashTo.y, dashProgress);

        printf("%f, %f\n", chocoShooterInstance.pos.x, chocoShooterInstance.pos.y);

        if (dashProgress >= 1.0f) 
            isDashing = FALSE;
    }

    if (CP_Input_KeyTriggered(KEY_H)) {  // Press 'H' to take 1 damage
        takeDamage(&chocoShooterInstance, boss1instance.attackPower);
    }
    if (CP_Input_KeyTriggered(KEY_G)) {  // Press 'L' to take 1 damage
        Damage(&boss1instance, chocoShooterInstance.attackPower);
    }
    // Check if 'G' key is pressed to fill the buff meter
    if (CP_Input_KeyTriggered(KEY_O)) {
        update_buff_meter(&chocoShooterInstance, dt);
    }

    // Buff meter drawing logic
    float meterX = CP_System_GetWindowWidth() - 150;
    float meterY = CP_System_GetWindowHeight() - 50;
    float meterWidth = 100;
    float meterHeight = 20;

    // Draw the empty part of the meter (white)
    CP_Settings_RectMode(CP_POSITION_CORNER);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Graphics_DrawRect(meterX, meterY, meterWidth, meterHeight);

    // Draw the filled part of the meter (blue)
    float fillWidth = get_buff_meter_fill();
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255));
    CP_Graphics_DrawRect(meterX, meterY, fillWidth, meterHeight);

    
    update_flicker(&playerFlickerState, dt);
    update_flicker(&bossFlickerState, dt);

    char healthDisplay[256];
    sprintf_s(healthDisplay, sizeof(healthDisplay), "Health: %d", chocoShooterInstance.health);
    CP_Settings_TextSize(30);
    CP_Font_DrawText(healthDisplay, 50, 50);

    // Boss follows player and changes Movement
    CP_Vector directionToPlayer;
    directionToPlayer.x = chocoShooterInstance.pos.x - boss1instance.pos.x;
    directionToPlayer.y = chocoShooterInstance.pos.y - boss1instance.pos.y;

    // Check horizontal Movement to flip boss's image
    if (directionToPlayer.x < 0) {
        boss1instance.isFlipped = TRUE;  // Face MOVING_LEFT if player is to the MOVING_LEFT
    }
    else {
        boss1instance.isFlipped = FALSE; // Face MOVING_RIGHT if player is to the MOVING_RIGHT
    }

    // Calculate distance and normalize Movement vector
    float distance = CP_Vector_Length(directionToPlayer);
    if (distance != 0) {
        directionToPlayer = CP_Vector_Scale(directionToPlayer, boss1instance.speed * dt / distance);
    }

    // Update boss pos toward the player
    boss1instance.pos.x += directionToPlayer.x;
    boss1instance.pos.y += directionToPlayer.y;

    // Draw player
   
  
        // Draw the player if visible
    if (is_entity_visible(&playerFlickerState)) {
        float drawWidth = chocoShooterInstance.isFlipped ? -chocoShooterInstance.width : chocoShooterInstance.width;
        CP_Image_Draw(
            chocoShooterInstance.img,
            chocoShooterInstance.pos.x,
            chocoShooterInstance.pos.y,
            drawWidth,
            chocoShooterInstance.height,
            255
        );
    }

    // Draw the boss if visible
    if (is_entity_visible(&bossFlickerState)) {
        float bossDrawWidth = boss1instance.isFlipped ? boss1instance.width : -boss1instance.width;
        CP_Image_Draw(
            boss1instance.img,
            boss1instance.pos.x,
            boss1instance.pos.y,
            bossDrawWidth,
            boss1instance.height,
            255
        );
    }

 if (chocoShooterInstance.health <= 0) {
        // Draw the Game Over image in the center of the screen
        float gameOverX = CP_System_GetWindowWidth() / 2;
        float gameOverY = CP_System_GetWindowHeight() / 2;
        CP_Image_Draw(gameOverImage, gameOverX, gameOverY, 1000, 600, 255); // Adjust size as needed
    }

	// Exit to Main Menu
	if (CP_Input_KeyTriggered(KEY_Q)) {
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	}
    float mouse_x = CP_Input_GetMouseWorldX();
    float mouse_y = CP_Input_GetMouseWorldY();
    CP_Image_Draw(chococursor, mouse_x, mouse_y, 70, 50, 255);
}

void game_exit(void)
{
    CP_Image_Free(&chocoShooterInstance.img);
    CP_Image_Free(&boss1instance.img);
}


int levels(void) {
    int lvl = 1;
    int enemies = 0;
    if (lvl == 1 && enemies == 0) {
        return 1;
    }
    else if (lvl == 1 && enemies != 0) {
        return 0;
    }
    return 0;
}



int shooting_seq(void)
{
    CP_Color color = CP_Color_Create(139, 69, 19, 255); // Brown color with full opacity
    CP_Settings_Fill(color);
     CP_Graphics_DrawCircle(100, 100, 50);
}


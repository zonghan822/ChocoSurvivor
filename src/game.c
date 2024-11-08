#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

#include "cprocessing.h"
#include "utils.h"
#include "hittest.h"
#include "mainmenu.h"
#include "player.h"
#include "enemy.h"
#include "boss.h"

#define IMAGE_POSITION CP_POSITION_CENTER
#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define MAX_ENEMY_COUNT 100

struct ChocoRoach roaches[MAX_ENEMY_COUNT];
struct Boss boss1instance;
struct ChocoShooter chocoShooterInstance;

int windowWidth, windowHeight, roachesCnt;

bool isDashing = false;
float dashProgress, lastDash;
CP_Vector dashTo;
const float DASH_COOLDOWN = 2.0, DASH_DISTANCE = 150.0, DASH_SPEED = 10.0;

void game_init(void)
{
    // Set to draw image from center
    CP_Settings_ImageMode(IMAGE_POSITION);

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
			.img = CP_Image_Load("Assets/choco_roach.png"),
			.pos.x = -35.f,
			.pos.y = CP_Random_RangeFloat(spawnMinY, spawnMaxY), // Generate random y pos for enemies
			.width = 100.f,
			.height = 150.f,
			.speed = 350.f,
			.health = 100,
			.dmg = 10,
			.movement = MOVING_RIGHT,
			.current_state = STATE_MOVING
		};

		// Store enemies in array
		roaches[i] = chocoRoach;
	}

    // Initialize boss instance
    boss1instance.height = 500.0f;
    boss1instance.width = 500.0f;
    boss1instance.speed = 80.0f;
    boss1instance.maxHealth = 40;
    boss1instance.pos.x = 100.0f;
    boss1instance.pos.y = 450.0f;
    boss1instance.img = CP_Image_Load("Assets/boss_lvl1.png");
    boss1instance.isFlipped = false;

    // Initialize player instance
    chocoShooterInstance.pos.x = 100.0f;
    chocoShooterInstance.pos.y = 100.0f;
    chocoShooterInstance.width = 100.0f,
    chocoShooterInstance.height = 150.0f,
    chocoShooterInstance.speed = 200.0f,
    chocoShooterInstance.health = 3,
    chocoShooterInstance.maxHealth = 3,
    chocoShooterInstance.img = CP_Image_Load("Assets/choco_shooter.png");
    chocoShooterInstance.isFlipped = false,
    chocoShooterInstance.movement = MOVING_RIGHT;
}


void game_update(void)
{
	// Clear background with grey background colour
	CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

	float dt = CP_System_GetDt();
    
    // Chocoroaches (Enemies)
	for (int i = 0; i < roachesCnt; i++) {

        // Enemy movement logic and attacking logic
        // Check enemy near with player
        if (AreRectsIntersecting(roaches[i].pos.x, roaches[i].pos.y, roaches[i].width * 2.f, roaches[i].height * 2.f, IMAGE_POSITION,
            chocoShooterInstance.pos.x, chocoShooterInstance.pos.y, chocoShooterInstance.width * 2.f, chocoShooterInstance.height * 2.f, IMAGE_POSITION)) {
            
            if (AreRectsIntersecting(roaches[i].pos.x, roaches[i].pos.y, roaches[i].width, roaches[i].height, IMAGE_POSITION,
                chocoShooterInstance.pos.x, chocoShooterInstance.pos.y, chocoShooterInstance.width, chocoShooterInstance.height, IMAGE_POSITION)) {
                printf("Intersecting...\n");
                // Stop movement of enemy when collide
                roaches[i].movement = STOPPED;

                // Attack goes here...

            }
            else { 
                printf("Chasing...\n");
                chase_player(&roaches[i], chocoShooterInstance);
            }
        }
        else {
            printf("Far away from player...\n");
            roaches[i].movement = STOPPED;
        }

		// Update position of enemy
        update_pos_enemy(&roaches[i], dt);

        // Draw enemy instance
        render_enemy(roaches[i]);
		
	}

    // Control and move player with key WASD
    move_player(&chocoShooterInstance);

    // Update position of player
    update_pos_player(&chocoShooterInstance, dt);


    if (lastDash < DASH_COOLDOWN)
        lastDash += dt;

    // Dash movement with spacebar
    if (CP_Input_KeyTriggered(KEY_SPACE) && !isDashing && lastDash >= DASH_COOLDOWN)
    {
        dashTo = chocoShooterInstance.pos;

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
            dashTo.y -= get_diagonal_dist(DASH_DISTANCE);
            dashTo.x -= get_diagonal_dist(DASH_DISTANCE);
            break;
        case MOVING_UP | MOVING_RIGHT:
            dashTo.x += get_diagonal_dist(DASH_DISTANCE);
            dashTo.y -= get_diagonal_dist(DASH_DISTANCE);
            break;
        case MOVING_DOWN | MOVING_LEFT:
            dashTo.x -= get_diagonal_dist(DASH_DISTANCE);
            dashTo.y += get_diagonal_dist(DASH_DISTANCE);
            break;
        case MOVING_DOWN | MOVING_RIGHT:
            dashTo.x += get_diagonal_dist(DASH_DISTANCE);
            dashTo.y += get_diagonal_dist(DASH_DISTANCE);
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
        takeDamage(&chocoShooterInstance, 1);
    }

    char healthDisplay[100];
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
    float drawWidth = chocoShooterInstance.isFlipped ? -chocoShooterInstance.width : chocoShooterInstance.width;
    CP_Image_Draw(
        chocoShooterInstance.img,
        chocoShooterInstance.pos.x,
        chocoShooterInstance.pos.y,
        drawWidth,
        chocoShooterInstance.height,
        255
    );

    // Draw boss with flipping
    float bossDrawWidth = boss1instance.isFlipped ? -boss1instance.width : boss1instance.width;
    if (levels() == 1) {
        CP_Image_Draw(
            boss1instance.img,
            boss1instance.pos.x,
            boss1instance.pos.y,
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
    CP_Image_Free(&chocoShooterInstance.img);
    CP_Image_Free(&boss1instance.img);
    for (int i = 0; i < roachesCnt; i++) {
        CP_Image_Free(&roaches[i].img);
    }
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




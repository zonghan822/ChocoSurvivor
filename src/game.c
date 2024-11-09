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
#include "tower.h"
#include "hittest.h"

#define IMAGE_POSITION CP_POSITION_CENTER
#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define MAX_ENEMY_COUNT 100

struct ChocoRoach roaches[MAX_ENEMY_COUNT];
struct Boss bossInstance;
struct ChocoShooter chocoShooterInstance;
struct DefenseTarget towerInstance;

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
    float spawnMinY = (float)windowHeight / 10.0;
    float spawnMaxY = (float)windowHeight * 9.0 / 10.0;

	// Define enemy count
	roachesCnt = 4;

    // Initialize dash variables
    dashProgress = 0.0;
    lastDash = 2.1f;

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
    bossInstance.speed = 80.0f;
    bossInstance.maxHealth = 40;
    bossInstance.hitbox.pos = bossInstance.pos = CP_Vector_Set(100.0, 450.0);
    bossInstance.img = CP_Image_Load("Assets/boss_lvl1.png");
    bossInstance.isFlipped = FALSE;
    bossInstance.width = 500.0f;
    bossInstance.height = get_preserved_heightf((int)CP_Image_GetWidth(bossInstance.img),
        (int)CP_Image_GetHeight(bossInstance.img), bossInstance.width);
    bossInstance.attack = melee_attack;
    bossInstance.canAttack = can_melee_attack;
    bossInstance.hitbox.width = bossInstance.width;
    bossInstance.hitbox.height = bossInstance.height;

    // Initialize player instance
    chocoShooterInstance.pos.x = 100.0f;
    chocoShooterInstance.pos.y = 100.0f;
    chocoShooterInstance.speed = 200.0f,
    chocoShooterInstance.health = 3,
    chocoShooterInstance.maxHealth = 3,
    chocoShooterInstance.hitbox.pos = chocoShooterInstance.pos = CP_Vector_Set(100.0, 100.0);
    chocoShooterInstance.speed = 200.0f;
    chocoShooterInstance.img = CP_Image_Load("Assets/choco_shooter.png");
    chocoShooterInstance.isFlipped = FALSE,
    chocoShooterInstance.movement = STOPPED;
    chocoShooterInstance.hitbox.width = chocoShooterInstance.width = 200.0f;
    chocoShooterInstance.hitbox.height = chocoShooterInstance.height = get_preserved_heightf((int)CP_Image_GetWidth(chocoShooterInstance.img),
        (int)CP_Image_GetHeight(chocoShooterInstance.img), chocoShooterInstance.width);

    // Initialize tower instance
    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(1500, 500);
}


void game_update(void)
{
	// Clear background with grey background colour
	CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

    // Placeholder
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Graphics_DrawRect(towerInstance.pos.x, towerInstance.pos.y, towerInstance.width, towerInstance.height);
    CP_Graphics_DrawRect(bossInstance.hitbox.pos.x, bossInstance.hitbox.pos.y, bossInstance.hitbox.width, bossInstance.hitbox.height);
    
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

        if (dashProgress >= 1.0f) 
            isDashing = FALSE;
    }

    // TODO: Update hitbox of the enemies when proper graphic is added
    // Collision
    {
        // With roaches
        for (int i = 0; i < ARRAY_COUNT(roaches); i++) {
            struct ChocoRoach roach = roaches[i];
            struct Circle hitbox = { // PLACEHOLDER
                .diameter = roach.radius * 2,
                .pos = roach.pos
            };

            // Player
            if (are_circle_rect_intersecting(hitbox, chocoShooterInstance.hitbox)) {
                if (!player_take_damage(&chocoShooterInstance, 1)) {
                    if (!tower_revive_player(&towerInstance, &chocoShooterInstance)) {
                        // Game over
                        printf("Game over\n");
                    }
                    else {
                        // Whatever we want to do after player is revived
                        printf("Revived\n");
                    }
                }
                else {
                    // Invincibility effect
                    break;
                }
            }

            // Tower
            if (are_circle_rect_intersecting(hitbox, get_tower_hitbox(towerInstance))) {
                // Clamp enemy pos
                // clamp_player_pos(struct Rectangle* entity_hitbox, struct Rectangle obstacle);
                if (!tower_take_damage(&towerInstance, 1)) {
                    // Game over
                    printf("Game over\n");
                }
                else {
                    // Whatever we want to do after tower is damaged
                }
            }
        }

        // TODO: Add one for boss

        // Tower collision with player (impassable)
        if (are_rects_intersecting(chocoShooterInstance.hitbox, get_tower_hitbox(towerInstance)))
        {
            // Clamp player pos?
            // clamp_player_pos(struct Rectangle* entity_hitbox, struct Rectangle obstacle);
        }
    }

    // Boss attack
    if (bossInstance.canAttack(bossInstance, chocoShooterInstance)) {
        bossInstance.attack(bossInstance, &chocoShooterInstance);
    }

    // Update hitboxes
    update_rect_hitbox(&chocoShooterInstance.hitbox, chocoShooterInstance.width / 1.2,
        chocoShooterInstance.height / 1.2, chocoShooterInstance.pos); // Player
    update_rect_hitbox(&bossInstance.hitbox, bossInstance.width, bossInstance.height, bossInstance.pos); // Boss
    

    // Boss follows player and changes Movement
    CP_Vector directionToPlayer = {
        .x = chocoShooterInstance.pos.x - bossInstance.pos.x,
        .y = chocoShooterInstance.pos.y - bossInstance.pos.y
    };

    // Check horizontal Movement to flip boss's image
    if (directionToPlayer.x < 0) {
        bossInstance.isFlipped = TRUE;  // Face MOVING_LEFT if player is to the MOVING_LEFT
    }
    else {
        bossInstance.isFlipped = FALSE; // Face MOVING_RIGHT if player is to the MOVING_RIGHT
    }

    // Calculate distance and normalize Movement vector
    float distance = CP_Vector_Length(directionToPlayer);
    if (distance != 0) {
        directionToPlayer = CP_Vector_Scale(directionToPlayer, bossInstance.speed * dt / distance);
    }

    // Update boss pos toward the player
    bossInstance.pos.x += directionToPlayer.x;
    bossInstance.pos.y += directionToPlayer.y;

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
    float bossDrawWidth = bossInstance.isFlipped ? -bossInstance.width : bossInstance.width;
    if (levels() == 1) {
        CP_Image_Draw(
            bossInstance.img,
            bossInstance.pos.x,
            bossInstance.pos.y,
            bossDrawWidth,
            bossInstance.height,
            255
        );
    }

	// Exit to Main Menu
	if (CP_Input_KeyTriggered(KEY_Q)) {
		CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	}

    // Draw UI
    {
        // Health
        char healthDisplay[256];
        sprintf_s(healthDisplay, sizeof(healthDisplay), "Health: %d", chocoShooterInstance.health);
        CP_Settings_TextSize(60);
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
        CP_Font_DrawText(healthDisplay, 135, 40);

        // PLACEHOLDER
        {
            CP_Settings_TextSize(35);
            CP_Font_DrawText("BOSS", bossInstance.hitbox.pos.x, bossInstance.hitbox.pos.y);
            CP_Font_DrawText("TOWER", towerInstance.pos.x, towerInstance.pos.y);
        }
    }
}

void game_exit(void)
{
    CP_Image_Free(&chocoShooterInstance.img);
    CP_Image_Free(&bossInstance.img);
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




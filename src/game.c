#include "cprocessing.h"
#include <stdio.h>
#include <stdbool.h>
#include "game.h"
#include "hittest.h"

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
    .maxhealth =40,
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
    .position = {1300.0f, 100.0f},
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

void Game_Init(void) {
    CP_System_SetFrameRate(40);
    chocoShooterInstance.image = CP_Image_Load("Assets/choco_shooter.png");
    boss1instance.image = CP_Image_Load("Assets/boss_lvl1.png");
}

void Game_Update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(100, 100, 255, 255));
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
 
    printf("Health: %d", chocoShooterInstance.health);
    
    if (AreCirclesIntersecting(
        chocoShooterInstance.position.x, chocoShooterInstance.position.y, chocoShooterInstance.width/2,
        boss1instance.position.x, boss1instance.position.y, boss1instance.width/2)) {

        // Apply damage if they are intersecting
        takeDamage(&chocoShooterInstance, 1);
    }

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
    float bossDrawWidth = boss1instance.isFlipped ? boss1instance.width : -boss1instance.width;
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
}

void Game_Exit(void) {
    CP_Image_Free(&chocoShooterInstance.image);
    CP_Image_Free(&boss1instance.image);
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

int flicker(int damage)
{
    if (damage == 1)
    {

    }
}



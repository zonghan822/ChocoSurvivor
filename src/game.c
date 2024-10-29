#include "cprocessing.h"
#include <stdio.h>
#include <stdbool.h>

// Direction enum, player, and boss structs
enum Direction { UP, DOWN, LEFT, RIGHT };

struct boss1 {
    CP_Vector position;
    float width;
    float height;
    float speed;
    CP_Image image;
    char imageTag[50];
    bool isFlipped; // Track if the boss should be facing left
};

// Initialize boss instance
struct boss1 boss1instance = {
    .height = 500.0f,
    .width = 500.0f,
    .speed = 100.0f,
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
};

// Initialize player instance
struct chocoshooter chocoShooterInstance = {
    .position = {100.0f, 100.0f},
    .width = 200.0f,
    .height = 300.0f,
    .speed = 200.0f,
    .imageTag = "choco shooter.png",
    .isFlipped = false,
    .lastDirection = RIGHT
};

void Game_Init(void) {
    CP_System_SetFrameRate(40);
    chocoShooterInstance.image = CP_Image_Load("Assets/choco_shooter.png");
    boss1instance.image = CP_Image_Load("Assets/boss_lvl1.png");
}

void Game_Update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(100, 100, 255, 255));
    float dt = CP_System_GetDt();

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
}

void Game_Exit(void) {
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

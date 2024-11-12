#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "cprocessing.h"
#include "mainmenu.h"
#include "level1.h"
#include "player.h"
#include "enemy.h"
#include "tower.h"
#include "camera.h"
#include "hittest.h"
#include "flicker.h"
#include "barrier.h"

const float LEVEL_WIDTH = 2000.0f,
    LEVEL_HEIGHT = 1200.0f; 
CP_Image chococursor;
CP_Image gameOverImage;

#pragma region Enemy Properties
// ************************************* //
//           Spawn Properties            //
// ************************************* //
float spawnMinY = 0.0f, spawnMaxY = 0.0f;
float spawnDelay = 0.0f, spawnTime = 0.0f;
unsigned short int roachesMax = 0;
enum SpawnWave {
    WAVE_0,
    WAVE_1,
    WAVE_2,
    WAVE_3
};
char isWaveStart = 0;
// ************************************* //

struct ChocoRoach roaches[MAX_ENEMY_COUNT] = { 0 };
enum SpawnWave currentWave = WAVE_0;
float healthBarDisplayTime = 0.f;
#pragma endregion

#pragma region Player Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
enum BulletType bulletType = NORMAL_BULLET;
float bulletSpeed = 0.f, bulletDiameter = 0.f;
float lastShotTime = 0;
#pragma endregion
//barrierinthemakings
enum BarrierState currentState = BARRIER_IDLE;
CP_Vector previewPosition;
CP_Vector finalPosition;
float barrierWidth = 50.0f;
float barrierHeight = 100.0f;
const float barrier_rect_W = 50.0f, barrier_rect_H = 50.0f;

void level_one_init(void) {
    
    chococursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);

    gameOverImage = CP_Image_Load("Assets/gg.png");

    CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

    cam = init_camera(playerInstance, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

    playerInstance = init_player(CP_Vector_Set(300, 500), 350.0f, 100, 30, 10.f);

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.1, LEVEL_HEIGHT / 2);

    bulletType = NORMAL_BULLET;
    bulletSpeed = 1000.f;
    bulletDiameter = 20.f;

    spawnDelay = 5.0f; // Give player some buffer
    currentWave = WAVE_0;
    isWaveStart = 1;

    //set initial flickering states
    playerInstance.flickerState.isVisible = TRUE;
}

void level_one_update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    float dt = CP_System_GetDt();

    // Init enemy when wave start
    if (isWaveStart) {
        float enemySpeed = 0.f;
        int enemyMaxHealth = 0, enemyDmg = 0;

        switch (currentWave) {
        case WAVE_0:
            enemySpeed = 150.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 20;
            break;
        case WAVE_1:
            enemySpeed = 200.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
            break;
        case WAVE_2:
            enemySpeed = 200.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
            break;
        case WAVE_3:
            enemySpeed = 200.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
            break;
        }

        for (int i = 0; i < roachesMax; i++) {
            // Store enemies in array
            roaches[i] = init_enemy(LEVEL_HEIGHT * 1/5, LEVEL_HEIGHT * 4/5, 200.f, enemySpeed, enemyMaxHealth, enemyDmg);
        }
        isWaveStart = 0;
    }

    // Enemy spawning
    if (spawnTime < spawnDelay) {
        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "Enemy coming in %d !!!", (int)(spawnDelay - spawnTime));
        CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
        CP_Settings_TextSize(100.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Font_DrawText(buffer, (float)CP_System_GetWindowWidth() / 2, (float)CP_System_GetWindowHeight() * 1/5);
        spawnTime += dt;
        lastShotTime = 0; // disable shooting
    } else {
        for (int i = 0; roaches[i].currentState; i++) {

            // Enemy movement logic and attacking logic
            struct Circle radarRoach = {.pos = roaches[i].pos, .diameter = 5 * roaches[i].width}; // define enemy radar
            if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) { // Check enemy near with player
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) { // When collide...
                    roaches[i].currentState = STATE_ATTACKING;
                }
                else {
                    roaches[i].currentState = STATE_CHASE_PLAYER;
                }
            }
            else {
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) {
                    roaches[i].currentState = STATE_ATTACKING;
                }
                else {
                    roaches[i].currentState = STATE_MOVING; // Move to defense target
                }
            }
        }
    }

    lastShotTime += dt;
    if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
        if (lastShotTime >= (1 / playerInstance.attackSpeed)) { // control fire rate
            float mouse_x = CP_Input_GetMouseWorldX(),
                mouse_y = CP_Input_GetMouseWorldY();
            CP_Vector bulletDirection = CP_Vector_Subtract(CP_Vector_Set(mouse_x, mouse_y), get_cam_pos_rel(playerInstance.pos, cam));
            struct Bullet bullet = init_bullet(bulletType, bulletSpeed, bulletDiameter, playerInstance.attackPower, bulletDirection);
            shoot_bullet(&playerInstance, &bullet);
            lastShotTime = 0;
        }
    }
    
    move_player(&playerInstance, dt);
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    
    render_tower(towerInstance, cam);

    update_flicker(&playerInstance.flickerState, dt);

    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Update hitbox of player
    render_player(playerInstance, 255, cam);

    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        update_pos_bullet(&playerInstance.bullets[i], dt);
        render_bullet(playerInstance.bullets[i], cam);
    }

    for (int i = 0; i < roachesMax; i++) {
        
        if (roaches[i].currentState) { // if enemy is alive

            switch (roaches[i].currentState) {
            case STATE_MOVING:
                enemy_move_to_target(&roaches[i], get_tower_hitbox(towerInstance), dt);
                break;
            case STATE_CHASE_PLAYER:
                enemy_move_to_target(&roaches[i], playerInstance.hitbox, dt);
                break;
            case STATE_ATTACKING:
                roaches[i].moveDirection = CP_Vector_Zero();
                // more code here..
                break;
            case STATE_DETOUR:
                // A* Pathfinding here...
                break;
            case STATE_IDLE:
                roaches[i].moveDirection = CP_Vector_Zero();
                break;
            }

            CP_Vector hitboxPos = { roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(roaches[i].width), roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(roaches[i].height) };
            update_rect_hitbox(&roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Update hitbox of enemy
        
            for (int j = 0; j < MAX_BULLET_COUNT; j++) {
                if (playerInstance.bullets[j].isShooting) {
                    struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
                    if (are_circle_rect_intersecting(bullet, roaches[i].hitbox)) {
                        enemy_take_damage(&roaches[i], playerInstance.bullets[j].dmg);
                        playerInstance.bullets[j].isShooting = FALSE;
                    }
                }
            }
        } 
        // can use else statement here for dying animation...
    }
    sort_enemies(roaches, roachesMax); // Sort enemy based on pos asc
    render_enemies(roaches, cam, roachesMax); // Draw enemies instances
    for (int i = 0; roaches[i].currentState; i++) {
        if (roaches[i].isTakingDamage) {
            if (healthBarDisplayTime < ENEMY_HEALTHBAR_DISPLAY_TIME) {
                render_enemy_health_bar(&roaches[i], cam);
                healthBarDisplayTime += dt;
            }
            else {
                healthBarDisplayTime = 0;
                roaches[i].isTakingDamage = FALSE;
            }
        }
    }

    if (towerInstance.health <= 0) {
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
    

    int windowWidth = CP_System_GetWindowWidth();
    int windowHeight = CP_System_GetWindowHeight();
    float barrier_butt_x = windowWidth - 1450.0f;
    float barrier_butt_y = windowHeight - 100.0f;

    //create barrier picking button
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255));
    CP_Graphics_DrawRect(barrier_butt_x, barrier_butt_y, barrier_rect_W, barrier_rect_H);
    CP_Vector mousePosition(mouse_x, mouse_y);
    


    switch (currentState) {
    case BARRIER_IDLE:
        // Start preview mode with a key press, if we haven't reached max barriers
        if (barrierCount < MAX_BARRIERS && CP_Input_KeyTriggered(KEY_P)) {
            currentState = BARRIER_PREVIEW;
        }
        break;

    case BARRIER_PREVIEW:
        // Update the preview position to follow the mouse
        previewPosition = CP_Vector_Set(mouse_x, mouse_y);

        // Draw translucent preview rectangle
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 128)); // Semi-transparent blue
        CP_Graphics_DrawRect(previewPosition.x, previewPosition.y, barrierWidth, barrierHeight);

        // Place the barrier on mouse click
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            if (barrierCount < MAX_BARRIERS) {
                // Store the barrier in the array
                barriers[barrierCount].position = previewPosition;
                barriers[barrierCount].width = barrierWidth;
                barriers[barrierCount].height = barrierHeight;
                barrierCount++;
                currentState = BARRIER_IDLE;  // Return to idle to place another barrier
            }
        }
        break;

    }

    // Draw all placed barriers
    for (int i = 0; i < barrierCount; i++) {
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255)); // Solid blue
        CP_Graphics_DrawRect(barriers[i].position.x, barriers[i].position.y, barriers[i].width, barriers[i].height);
    }
    CP_Image_Draw(chococursor, mouse_x, mouse_y, 70, 50, 255);
}




void level_one_exit(void) {

}
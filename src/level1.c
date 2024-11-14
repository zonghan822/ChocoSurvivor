#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

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

float windowWidth, windowHeight;

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

enum AttackTarget {
    NONE,
    PLAYER,
    TOWER,
    BARRIER
};

struct ChocoRoach roaches[MAX_ENEMY_COUNT] = { 0 };
enum SpawnWave currentWave = WAVE_0;
enum AttackTarget enemyAttackTarget = NONE;
float healthBarDisplayTime = 0.f;
#pragma endregion

#pragma region Player Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
enum BulletType bulletType = NORMAL_BULLET;
float bulletSpeed = 0.f, bulletDiameter = 0.f;
float lastShotTime = 0;
bool enableShooting = false;
#pragma endregion

#pragma region Barriers Properties
enum BarrierState currentState = BARRIER_IDLE;
CP_Vector previewPosition;
CP_Vector finalPosition;
float barrierWidth = 0.0f;
float barrierHeight = 0.0f;
float barrier_rect_W = 0.0f, barrier_rect_H = 0.0f;
#pragma endregion

#pragma region Map Properties
struct Rectangle obstacles[100] = { 0 };
#pragma endregion


void level_one_init(void) {
    
    chococursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);
    gameOverImage = CP_Image_Load("Assets/gg.png");
    CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

    windowWidth = (float)CP_System_GetWindowWidth();
    windowHeight = (float)CP_System_GetWindowHeight();

    #pragma region Player_Init
    // Init Player Properties
    playerInstance = init_player(CP_Vector_Set(300, 500), 350.0f, 100, 30, 10.f);
    playerInstance.flickerState.isVisible = TRUE; //set initial flickering states
    bulletType = NORMAL_BULLET;
    bulletSpeed = 1000.f;
    bulletDiameter = 20.f;
    #pragma endregion

    #pragma region Defense_Target_Init
    // Init Defense Target Properties
    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.1, LEVEL_HEIGHT / 2);
    #pragma endregion

    #pragma region Barrier_Init
    // Init Barrier Properties
    currentState = BARRIER_IDLE;
    barrierWidth = 50.0f;
    barrierHeight = 100.0f;
    barrier_rect_W = 50.0f;
    barrier_rect_H = 50.0f;
    #pragma endregion

    cam = init_camera(playerInstance, windowWidth, windowHeight);

    // 0 vector is used to indicate end of array, use CP_POSITION_CENTER to avoid use of 0 vector 
    struct Rectangle obstacleTest1 = {.pos.x = LEVEL_WIDTH/2, .pos.y = LEVEL_HEIGHT*3/7, .width = 200, .height = 100};
    struct Rectangle obstacleTest2 = { .pos.x = LEVEL_WIDTH/2, .pos.y = LEVEL_HEIGHT*4/7, .width = 200, .height = 100 };
    obstacles[0] = obstacleTest1;
    obstacles[1] = obstacleTest2;

    spawnDelay = 5.0f; // Give player some buffer
    currentWave = WAVE_0;
    isWaveStart = 1;
}

void level_one_update(void) {

    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    float dt = CP_System_GetDt();
    float mouse_x = CP_Input_GetMouseWorldX();
    float mouse_y = CP_Input_GetMouseWorldY();

    enableShooting = true;

    // Init enemy when wave start
    if (isWaveStart) {
        #pragma region Enemy_Init
        float enemySpeed = 0.f;
        int enemyMaxHealth = 0, enemyDmg = 0;

        switch (currentWave) {
        case WAVE_0:
            enemySpeed = 150.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
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
            roaches[i] = init_enemy(LEVEL_HEIGHT * 1 / 5, LEVEL_HEIGHT * 4 / 5, 500.f, enemySpeed, enemyMaxHealth, enemyDmg);
        }
        #pragma endregion
        isWaveStart = 0;
    }

    // Enemy spawning
    if (spawnTime < spawnDelay) {
        #pragma region Show_Spawning_Time
        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "Enemy coming in %d !!!", (int)(spawnDelay - spawnTime));
        CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
        CP_Settings_TextSize(100.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Font_DrawText(buffer, (float)CP_System_GetWindowWidth() / 2, (float)CP_System_GetWindowHeight() * 1 / 5);
        #pragma endregion
        spawnTime += dt;
        enableShooting = false; // disable shooting
    } else {
        #pragma region Enemy_Movement_Logic
        for (int i = 0; roaches[i].currentState; i++) {

            // Enemy movement logic and attacking logic
            struct Circle radarRoach = { .pos = roaches[i].pos, .diameter = 5 * roaches[i].width }; // define enemy radar
            if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) { // Check enemy near with player
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) { // When collide...
                    roaches[i].currentState = STATE_ATTACKING;
                    enemyAttackTarget = PLAYER;
                }
                else {
                    roaches[i].currentState = STATE_CHASE_PLAYER;
                }
            }
            else {
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) {
                    roaches[i].currentState = STATE_ATTACKING;
                    enemyAttackTarget = PLAYER;
                }
                else {
                    roaches[i].currentState = STATE_MOVING; // Move to defense target
                }
            }
        }
        #pragma endregion
    }
    
    move_player(&playerInstance, dt);
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    
    #pragma region Place_Barriers
    float barrier_butt_x = windowWidth - 1450.0f;
    float barrier_butt_y = windowHeight - 100.0f;

    switch (currentState) {
    case BARRIER_IDLE:

        // Start preview mode with a key press, if we haven't reached max barriers
        if (get_barrier_count() < MAX_BARRIERS && CP_Input_KeyTriggered(KEY_P)) {
            currentState = BARRIER_PREVIEW;
        }
        break;

    case BARRIER_PREVIEW:
        enableShooting = false;
        // Update the preview position to follow the mouse
        previewPosition = get_world_pos_rel(CP_Vector_Set(mouse_x, mouse_y), cam);

        // Draw translucent preview rectangle
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 128)); // Semi-transparent blue
        CP_Graphics_DrawRect(mouse_x, mouse_y, barrierWidth, barrierHeight);

        // Place the barrier on mouse click
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            int barrierIndex = get_barrier_count();
            if (barrierIndex < MAX_BARRIERS) {
                // Set the barrier properties
                set_barrier_properties(barrierIndex, previewPosition, barrierWidth, barrierHeight, BARRIER_INITIAL_HEALTH);
                increment_barrier_count();
                currentState = BARRIER_IDLE;
            }
        }
        break;
    }
    #pragma endregion

    // Damage the first active barrier when pressing 'B'
    if (CP_Input_KeyTriggered(KEY_B)) {
        takeDamage(1);
    }

    #pragma region Shoot_Bullet
    lastShotTime += dt;
    if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT) && enableShooting) {
        if (lastShotTime >= (1 / playerInstance.attackSpeed)) { // control fire rate
            // bullet direction is the vector of player with mouse pointer
            CP_Vector bulletDirection = CP_Vector_Subtract(CP_Vector_Set(mouse_x, mouse_y), get_cam_pos_rel(playerInstance.pos, cam));
            struct Bullet bullet = init_bullet(bulletType, bulletSpeed, bulletDiameter, playerInstance.attackPower, bulletDirection);
            shoot_bullet(&playerInstance, &bullet);
            lastShotTime = 0;
        }
    }
    #pragma endregion
    
    #pragma region Render_Obstacles
    CP_Settings_Fill(CP_Color_Create(150, 150, 150, 255));
    for (int i = 0; obstacles[i].width != 0; i++) {
        CP_Vector vec = get_cam_pos_rel(obstacles[i].pos, cam);
        CP_Graphics_DrawRect(vec.x, vec.y, obstacles[i].width, obstacles[i].height);
    }
    #pragma endregion

    // Render Defense Target
    render_tower(towerInstance, cam);

    // Update and render player
    update_flicker(&playerInstance.flickerState, dt);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Update hitbox of player
    render_player(playerInstance, 255, cam);

    #pragma region Render_Barriers
    // Draw all placed barriers
    struct Barrier* barriers = get_barriers();
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255)); // Solid blue
    for (int i = 0; i < get_barrier_count(); i++) {
        if (barriers[i].health > 0) {  // Only draw if health is greater than zero
            CP_Vector vec = get_cam_pos_rel(barriers[i].position, cam);
            CP_Graphics_DrawRect(vec.x, vec.y, barriers[i].width, barriers[i].height);
        }
    }
    #pragma endregion

    // Update and render bullets
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        update_pos_bullet(&playerInstance.bullets[i], dt);
        render_bullet(playerInstance.bullets[i], cam);
    }

    #pragma region Get_All_Clips
    struct Barrier* barriers = get_barriers();
    // get all clips position
    struct Rectangle clips[100] = { 0 };
    int clipCount = 0, obstacleCount = 0;
    while (obstacles[obstacleCount].width != 0) {
        clips[clipCount] = obstacles[obstacleCount];
        obstacleCount = ++clipCount;
    }
    while (barriers->health) {
        clips[clipCount] = (barriers++)->hitbox;
        clipCount++;
    }
    #pragma endregion

    for (int i = 0; i < roachesMax; i++) {   
        if (roaches[i].currentState) { // if enemy is alive

            #pragma region Enemy_Action
            switch (roaches[i].currentState) {
            case STATE_MOVING:{
                struct Circle radarRoach = { .pos = roaches[i].pos, .diameter = 5 * roaches[i].width }; // define enemy radar
                int clipIndex = 0;
                while (clipIndex < clipCount) {
                    if (are_circle_rect_intersecting(radarRoach, clips[clipIndex])) { // Check enemy near with obstacles or barriers
                        // create a temporary rectangle to simulate the movement to defense target
                        int step = 4;
                        CP_Vector moveDirection = CP_Vector_Scale(CP_Vector_Subtract(towerInstance.pos, roaches[i].hitbox.pos), 1.f/ (float)step);
                        struct Rectangle tempRect = { roaches[i].hitbox.pos, roaches[i].hitbox.width, roaches[i].hitbox.height};
                        for (int j = 0; j < step; j++) {
                            tempRect.pos = CP_Vector_Add(tempRect.pos, moveDirection);
                            if (are_rects_intersecting(tempRect, clips[clipIndex])) { // Clip blocking the way...
                                struct Rectangle radarClip = { .width = clips[clipIndex].width + (2 * roaches[i].hitbox.width),
                                                               .height = clips[clipIndex].height + (2 * roaches[i].hitbox.height),
                                                               .pos = clips[clipIndex].pos };
                                int nearbyClipIdx = 0;
                                while()
                                    if (are_rects_intersecting(clips[nearbyClipIdx], radarClip)) {
                                        break;
                                    };

                               // for clips 
                                //
                            }
                        }
                        break;
                    }
                    clipIndex++;
                }
                if (clipIndex == clipCount) { // if no obstacles...
                    enemy_move_to_target(&roaches[i], get_tower_hitbox(towerInstance), dt);
                }
                break;
            }
                
            case STATE_CHASE_PLAYER: {
                struct Circle radarRoach = { .pos = roaches[i].pos, .diameter = 5 * roaches[i].width }; // define enemy radar
                int clipIndex = 0;
                while (clipIndex < clipCount) {
                    if (are_circle_rect_intersecting(radarRoach, clips[clipIndex])) { // Check enemy near with obstacles or barriers
                        break;
                    }
                    clipIndex++;
                }
                if (clipIndex == clipCount) { // if no obstacles...
                    enemy_move_to_target(&roaches[i], playerInstance.hitbox, dt);
                }
                break;
            }

            case STATE_ATTACKING: {
                roaches[i].moveDirection = CP_Vector_Zero();

                // attack animation here...

                // if animation end then...
                switch (enemyAttackTarget) {
                case PLAYER:
                    //playerTakeDamage...
                    break;
                case TOWER:
                    //towerTakeDamage...
                    break;
                case BARRIER:
                    //barrierTakeDamage...
                    break;
                }
                break;
            }
                
            case STATE_IDLE:
                roaches[i].moveDirection = CP_Vector_Zero();
                break;
            }

            CP_Vector hitboxPos = { roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(roaches[i].width), roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(roaches[i].height) };
            update_rect_hitbox(&roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Update hitbox of enemy
            #pragma endregion
        
            #pragma region Enemy_Takes_Damage
            for (int j = 0; j < MAX_BULLET_COUNT; j++) {
                if (playerInstance.bullets[j].isShooting) {
                    struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
                    if (are_circle_rect_intersecting(bullet, roaches[i].hitbox)) {
                        enemy_take_damage(&roaches[i], playerInstance.bullets[j].dmg);
                        playerInstance.bullets[j].isShooting = FALSE;
                    }
                }
            }
            #pragma endregion
        } 
        // can use else statement here for dying animation...
    }
    sort_enemies(roaches, roachesMax); // Sort enemy based on pos asc
    render_enemies(roaches, cam, roachesMax); // Draw enemies instances
    #pragma region Render_Enemy_HealthBar
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
    #pragma endregion

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

    // Draw cursor (must be placed last)
    CP_Image_Draw(chococursor, mouse_x, mouse_y, 70, 50, 255);
}


void level_one_exit(void) {

}
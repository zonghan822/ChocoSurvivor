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
#include "enemy.h"
#include "utils.h"
#include "flicker.h"
#include "barrier.h" 
#include "astar.h"

#define MAX_ENEMY_COUNT 100
#define LEVEL_WIDTH 2000.0f
#define LEVEL_HEIGHT 900.0f

CP_Image levelBg;
CP_Image chococursor;
CP_Image gameOverImage;

float windowWidth, windowHeight;

#pragma region Spawn Properties
static float spawnMinY = 0.0f, spawnMaxY = 0.0f;
static float spawnDelay = 0.0f, spawnTime = 0.0f;
enum SpawnWave {
    WAVE_0,
    WAVE_1,
    WAVE_2,
    WAVE_3
};
static enum SpawnWave currentWave = WAVE_0;
static char isWaveStart = 0;
#pragma endregion

#pragma region Enemy Properties
enum AttackTarget {
    NONE,
    PLAYER,
    TOWER,
    BARRIER
};
enum AttackTarget enemyAttackTarget = NONE;
static unsigned short int roachesMax = 0;
static struct ChocoRoach roaches[MAX_ENEMY_COUNT];
float healthBarDisplayTime = 0.f;
int enemyKilled = 0;
#pragma endregion

#pragma region Player_Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
enum BulletType bulletType = NORMAL_BULLET;
float bulletSpeed = 0.f, bulletDiameter = 0.f;
float lastShotTime = 0;
bool enableShooting = false;
#pragma endregion

#pragma region HP/BUFF bar properties
float meterX = 0;
float meterY = 0;
float meterWidth = 0;
float meterHeight = 0;
int exp_perkill = 0; 
int current_exp = 0;
float fillSpeed = 0.0f;
float fillWidth = 0;         
float targetFillWidth = 0;   
int activeBuffSlot = 0;          // 0 means no active buff, 1-3 represent the buff slots
//float activeBuffDuration = 0.0f;  // Duration in seconds for the active buff (10s when active)
#pragma endregion

#pragma region Obstacle/Barrier Properties
struct Rectangle obstacles[80] = { 0 };
struct Rectangle playerBarricades[10];
enum BarrierState currentState = BARRIER_IDLE;
CP_Vector previewPosition;
CP_Vector finalPosition;
float barrierWidth = 0.0f;
float barrierHeight = 0.0f;
float barrier_rect_W = 0.0f, barrier_rect_H = 0.0f;
#pragma endregion

#pragma region Map Properties
struct Rectangle worldClips[100];
struct Rectangle playerClips[1];
struct Rectangle enemyClips[1];
int worldClipsCount = 0;
PathNode grid[1000];
int nodeCount = 0;
#pragma endregion


void level_one_init(void) {
    levelBg = CP_Image_Load("Assets/Levels/level1.png");
    
    chococursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);
    gameOverImage = CP_Image_Load("Assets/gg.png");
    CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

    windowWidth = (float)CP_System_GetWindowWidth();
    windowHeight = (float)CP_System_GetWindowHeight();

    #pragma region Player_Init
    // Init Player Properties
    playerInstance = init_player(CP_Vector_Set(towerInstance.pos.x / 1.05, towerInstance.pos.y), 350.0f, 100, 30, 10.f);
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
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.05, LEVEL_HEIGHT / 2);
    #pragma endregion

    #pragma region Barrier_Init
    // Init Barrier Properties
    currentState = BARRIER_IDLE;
    barrierWidth = 50.0f;
    barrierHeight = 100.0f;
    barrier_rect_W = 50.0f;
    barrier_rect_H = 50.0f;
    #pragma endregion

    empty_enemies(roaches);
    enemyKilled = 0;
    
    // TODO 3: exp_perkill is placeholder
    exp_perkill = 6;//exp for buff bar per roachy
    current_exp = 0;
    meterX = windowWidth - 150;//  buff bar properties
    meterY = windowHeight - 50;
    meterWidth = 100;
    meterHeight = 20;
    fillSpeed = 0.05f;

    cam = init_camera(playerInstance, windowWidth, windowHeight);

    // Define obstacles for testing
    // 0 vector is used to indicate end of array, use CP_POSITION_CENTER to avoid use of 0 vector 
    struct Rectangle obstacleTest1 = {.pos.x = LEVEL_WIDTH/2, .pos.y = LEVEL_HEIGHT*3/7, .width = 200, .height = 100};
    struct Rectangle obstacleTest2 = { .pos.x = LEVEL_WIDTH/2, .pos.y = LEVEL_HEIGHT*4/7, .width = 200, .height = 100 };
    obstacles[0] = obstacleTest1;
    obstacles[1] = obstacleTest2;

    spawnDelay = 5.0f; // Give player some buffer
    currentWave = WAVE_0;
    isWaveStart = 1;

    // store non-movable path in worldClips
    worldClips[0] = get_tower_hitbox(towerInstance);
    worldClipsCount = 1;
    for (int i = 0; obstacles[i].width; i++) {
        worldClips[worldClipsCount++] = obstacles[i];
    }

    nodeCount = init_grid(grid, LEVEL_WIDTH, LEVEL_HEIGHT, playerInstance.width, playerInstance.height);
    update_grid(grid, worldClips, worldClipsCount);
}

void level_one_update(void) {

    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

    /*******************************
        Render background image
    ********************************/
    CP_Vector bgVec = CP_Vector_Set((LEVEL_WIDTH / 2), (LEVEL_HEIGHT / 2)),
        relBgVec = get_cam_pos_rel(bgVec, cam);
    CP_Image_Draw(levelBg, relBgVec.x, relBgVec.y, LEVEL_WIDTH, LEVEL_HEIGHT, 255);

    float dt = CP_System_GetDt();
    float mouse_x = CP_Input_GetMouseWorldX();
    float mouse_y = CP_Input_GetMouseWorldY();

    enableShooting = true;
    draw_grid(grid, nodeCount, cam);
    // Init enemy when wave start
    if (isWaveStart) {
        #pragma region Enemy_Init
        float enemySpeed = 0.f;
        int enemyMaxHealth = 0, enemyDmg = 0;

        switch (currentWave) {
        case WAVE_0:
            enemySpeed = 100.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
            break;
        case WAVE_1:
            enemySpeed = 100.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
            break;
        case WAVE_2:
            enemySpeed = 100.f;
            enemyMaxHealth = 100;
            enemyDmg = 20;
            roachesMax = 40;
            break;
        case WAVE_3:
            enemySpeed = 100.f;
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

            // Enemy move to player if near to player, else move to defense target
            struct Circle radarRoach = { .pos = roaches[i].pos, .diameter = 5 * roaches[i].width }; // define enemy radar
            if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) { // Check enemy near with player
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) { // When collide attack player
                    roaches[i].currentState = STATE_ATTACKING;
                    enemyAttackTarget = PLAYER;
                }
                //else if (// barrierAttack) {
                //    roaches[i].currentState = STATE_ATTACKING;
                //    enemyAttackTarget = BARRIER;
                //}
                else {
                    roaches[i].currentState = STATE_CHASE_PLAYER;
                }
            }
            else {
                if (are_rects_intersecting(roaches[i].hitbox, get_tower_hitbox(towerInstance))) {
                    roaches[i].currentState = STATE_ATTACKING;
                    enemyAttackTarget = TOWER;
                }
                //else if (// barrierAttack) { 
                //    roaches[i].currentState = STATE_ATTACKING;
                //    enemyAttackTarget = BARRIER;
                //}
                else {
                    roaches[i].currentState = STATE_MOVING; // Move to defense target
                }
            }
        }
        #pragma endregion
    }
    
    move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT);

    // Prevent player and bullets move into non-movable path
    for (int i = 0; i < worldClipsCount; i++) {
        struct Rectangle worldClip = worldClips[i];
        if (are_rects_intersecting(playerInstance.hitbox, worldClip))
            clamp_pos_clip_rel(&playerInstance.pos, playerInstance.hitbox, worldClip);
        for (int j = 0; j < MAX_BULLET_COUNT; j++) {
            if (playerInstance.bullets[j].isShooting) {
                struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
                if (are_circle_rect_intersecting(bullet, worldClip)) {
                    playerInstance.bullets[j].isShooting = FALSE;
                }
            }
        }
    }

    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    
    #pragma region Place_Barriers
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
        CP_Settings_RectMode(CP_POSITION_CENTER);
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

                // store the barrier in worldClips
                struct Barrier* barriers = get_barriers();
                worldClips[worldClipsCount] = barriers[barrierIndex].hitbox;
                update_grid(grid, worldClips, worldClipsCount);
            }
        }
        break;
    }
    #pragma endregion

    // Damage the first active barrier when pressing 'B'
    if (CP_Input_KeyTriggered(KEY_B)) {
        barrier_take_damage(1);
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
    CP_Settings_RectMode(CP_POSITION_CENTER);
    CP_Settings_Fill(CP_Color_Create(150, 150, 150, 255));
    for (int i = 0; obstacles[i].width; i++) {
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

    for (int i = 0; i < roachesMax; i++) {   
        if (roaches[i].currentState) { // if enemy is alive

            #pragma region Enemy_Action
            switch (roaches[i].currentState) {
            case STATE_MOVING:{
                enemy_move_to_target(&roaches[i], get_tower_hitbox(towerInstance), dt);
                break;
            }
                
            case STATE_CHASE_PLAYER: {
                enemy_move_to_target(&roaches[i], playerInstance.hitbox, dt);
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
                        enemyKilled += enemy_take_damage(&roaches[i], playerInstance.bullets[j].dmg);
                        playerInstance.bullets[j].isShooting = FALSE;
                    }
                }
            }
            #pragma endregion
        } 
        // can use else statement here for dying animation...
    }

    // Buff and currency drops here...
    

    // Draw the empty part of the meter (white)
    CP_Settings_RectMode(CP_POSITION_CORNER);

    if (enemyKilled) {
        int expGained = exp_perkill * enemyKilled;
        enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained);

        // Apply the buff if one was granted
        if (randomBuff != 0) {
            apply_buff(&playerInstance, randomBuff);
        }

        // Reset enemyKilled count
        enemyKilled = 0;
    }
    
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Graphics_DrawRect(meterX, meterY, meterWidth, meterHeight);
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255));
    CP_Graphics_DrawRect(meterX, meterY, fillWidth, meterHeight);


    CP_Settings_TextSize(20.0f);

    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    // buff slot 1 
    CP_Graphics_DrawCircle(150,750,80); 
    //buff slot 2
    CP_Graphics_DrawCircle(300, 750, 80); 
    // buff slot 3
    CP_Graphics_DrawCircle(450, 750, 80);
        
    // Display the active buff slot
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Graphics_DrawCircle(600, 750, 100);

    for (int i = 0; i < 3; i++) {
        // Set the position of each buff slot circle
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));


        float slotPositions[] = { 150, 300, 450 }; // Positions for buff slots

        for (int i = 0; i < 3; i++) {
            switch (playerInstance.buffs.type[i]) {
            case ATTACK_UP:
                CP_Font_DrawText(
                    playerInstance.buffs.attackP_lvl > 3 ? "A Max Lvl" :
                    playerInstance.buffs.attackP_lvl == 1 ? "A Lvl 1" :
                    playerInstance.buffs.attackP_lvl == 2 ? "A Lvl 2" : "A Lvl 3",
                    slotPositions[i], 750);
                break;
            case ATTACK_SPEED_UP:
                CP_Font_DrawText(
                    playerInstance.buffs.attackS_lvl > 3 ? "AS Max Lvl" :
                    playerInstance.buffs.attackS_lvl == 1 ? "AS Lvl 1" :
                    playerInstance.buffs.attackS_lvl == 2 ? "AS Lvl 2" : "AS Lvl 3",
                    slotPositions[i], 750);
                break;
            case HEALTH_UP:
                CP_Font_DrawText(
                    playerInstance.buffs.health_lvl > 3 ? "HP Max Lvl" :
                    playerInstance.buffs.health_lvl == 1 ? "HP Lvl 1" :
                    playerInstance.buffs.health_lvl == 2 ? "HP Lvl 2" : "HP Lvl 3",
                    slotPositions[i], 750);
                break;
            default:
                CP_Font_DrawText("Empty", slotPositions[i], 750);
                break;
            }

            if (CP_Input_KeyTriggered(KEY_1) && playerInstance.buffs.type[0] != NO_BUFF) {
                activate_buff_from_slot(&playerInstance, 1);
            }
            if (CP_Input_KeyTriggered(KEY_2) && playerInstance.buffs.type[1] != NO_BUFF) {
                activate_buff_from_slot(&playerInstance, 2);
            }
            if (CP_Input_KeyTriggered(KEY_3) && playerInstance.buffs.type[2] != NO_BUFF) {
                activate_buff_from_slot(&playerInstance, 3);
            }
            if (playerInstance.activeBuffType != NO_BUFF) {
                const char* activeBuffName;

                // Determine the name based on the active buff type
                switch (playerInstance.activeBuffType) {
                case ATTACK_UP:
                    activeBuffName = "ATTACK UP";
                    break;
                case ATTACK_SPEED_UP:
                    activeBuffName = "ATTACK SPEED UP";
                    break;
                case HEALTH_UP:
                    activeBuffName = "HEALTH UP";
                    break;
                default:
                    activeBuffName = "NO BUFF";
                    break;
                }

                
                CP_Font_DrawText(activeBuffName, 600, 750);  
                if (playerInstance.activeBuffLevel == 1) {
                    CP_Font_DrawText("Level 1", 600, 770);
                }
                else if (playerInstance.activeBuffLevel == 2) {
                    CP_Font_DrawText("Level 2", 600, 770);
                }
                else if (playerInstance.activeBuffLevel == 3) {
                    CP_Font_DrawText("Level 3", 600, 770);
                }
                else if (playerInstance.activeBuffLevel > 3) {
                    CP_Font_DrawText("Max Level", 600, 770);
                }
            }
        }
    }
    // Set targetFillWidth based on updated buffExp
    targetFillWidth = ((playerInstance.buffs.buffExp % playerInstance.buffs.maxBuffMeter) / (float)playerInstance.buffs.maxBuffMeter) * meterWidth;
    fillWidth = CP_Math_ClampFloat(fillWidth, 0, playerInstance.buffs.maxBuffMeter);

    // TODO:
    // fillSpeed *= dt;

    // TODO 2:
    // Max is 100
    // Value = 99
    // + 5 = 104
    // Clamp to 100
    // Delay
    // Then take 104 - 100 = 4, assign it to the value
    // Different color when bar reaches max (just use if statement)

    // Smoothly interpolate fillWidth towards targetFillWidth
    if (fillWidth < targetFillWidth) {
        fillWidth += fillSpeed * (targetFillWidth - fillWidth);
        if (fillWidth > targetFillWidth) fillWidth = targetFillWidth;  // Prevent overshooting
    }
    else if (fillWidth > targetFillWidth) {
        fillWidth -= fillSpeed * (fillWidth - targetFillWidth);
        if (fillWidth < targetFillWidth) fillWidth = targetFillWidth;  // Prevent undershooting
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
        float gameOverX = windowWidth / 2;
        float gameOverY = windowHeight / 2;
        CP_Image_Draw(gameOverImage, gameOverX, gameOverY, 1000, 600, 255); // Adjust size as needed
    }

    #pragma region Render_UI
    {
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));

        // Health
        CP_Settings_TextSize(25);
        char healthStr[16];
        sprintf_s(healthStr, sizeof(healthStr), "Health: %d", playerInstance.health);
        CP_Vector playerVec = get_cam_pos_rel(playerInstance.pos, cam);
        float playerHalfHeight = playerInstance.height / 2,
            healthX = playerVec.x,
            healthY = (playerVec.y - playerHalfHeight) - 15.0f;
        CP_Settings_TextSize(25);
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
        CP_Font_DrawText(healthStr, healthX, healthY);

        // Wave
        CP_Settings_TextSize(60);
        char currentWaveStr[8];
        sprintf_s(currentWaveStr, sizeof(currentWaveStr), "Wave: %d", 1);
        CP_Font_DrawText(currentWaveStr, CP_System_GetWindowWidth() / 2.0f, 50.0f);
    }
    #pragma endregion
    
    // Exit to Main Menu
    if (CP_Input_KeyTriggered(KEY_Q)) {
        CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
    }

    // Draw cursor (must be placed last)
    CP_Image_Draw(chococursor, mouse_x, mouse_y, 70, 50, 255);
}


void level_one_exit(void) {
    CP_Image_Free(&playerInstance.img);
    CP_Image_Free(&levelBg);

    for (int i = 0; i < roachesMax; i++) {
        struct ChocoRoach roach = roaches[i];
        if (roach.img != NULL)
            CP_Image_Free(&roach.img);
    }
    CP_Image_Free(&chococursor);
}
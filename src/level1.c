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
int enemyKilled = 0;
#pragma endregion

#pragma region Player Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
enum BulletType bulletType = NORMAL_BULLET;
float bulletSpeed = 0.f, bulletDiameter = 0.f;
float lastShotTime = 0;
#pragma endregion

#pragma region HP/BUFF bar properties
float meterX =0 ;
float meterY = 0 ;
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


void level_one_init(void) {

    //to work for active buff to set as 0 at the start for all attributes.
  

    chococursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
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

    int exp_perkill = 6;//exp for buff bar per roachy
    int current_exp = 0;
    float meterX = CP_System_GetWindowWidth() - 150;//  buff bar properties
    float meterY = CP_System_GetWindowHeight() - 50;
    float meterWidth = 100;
    float meterHeight = 20;
    float fillSpeed = 0.05f;
  
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
                        enemyKilled += enemy_take_damage(&roaches[i], playerInstance.bullets[j].dmg);
                        playerInstance.bullets[j].isShooting = FALSE;
                    }
                }
            }
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

                // Display the active buff type and level at a specific position
                CP_Font_DrawText(activeBuffName, 600, 750);  // Adjust coordinates as needed
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
    CP_Image_Draw(chococursor, mouse_x, mouse_y, 70, 50, 255);
}

void level_one_exit(void) {

}
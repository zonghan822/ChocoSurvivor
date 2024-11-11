#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "level1.h"
#include "player.h"
#include "tower.h"
#include "cprocessing.h"
#include "camera.h"
#include "hittest.h"
#include "enemy.h"
#include "utils.h"

#define MAX_ENEMY_COUNT 100
#define LEVEL_WIDTH 2000.0f
#define LEVEL_HEIGHT 900.0f

CP_Image levelBg;

// ************************************* //
// *         Spawn Properties          * //
// ************************************* //
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

// ************************************* //
// *         Enemy Properties          * //
// ************************************* //
static unsigned short int roachesMax = 0;
static struct ChocoRoach roaches[MAX_ENEMY_COUNT];

// ************************************* //
// *        Player Properties          * //
// ************************************* //
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
static enum BulletType bulletType = NORMAL_BULLET;
static float bulletSpeed = 0.f, bulletDiameter = 0.f;
static float lastShotTime = 0;

// ************************************* //
// *         Obstacles / Clips         * //
// ************************************* //
struct Rectangle worldClips[1];
struct Rectangle playerClips[1];
struct Rectangle enemyClips[1];
struct Rectangle playerBarricades[10];

void level_one_init(void) {
    levelBg = CP_Image_Load("Assets/Levels/level1.png");
    cam = init_camera(playerInstance, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.05, LEVEL_HEIGHT / 2);
    worldClips[0] = get_tower_hitbox(towerInstance);

    playerInstance = init_player(CP_Vector_Set(towerInstance.pos.x / 1.05, towerInstance.pos.y), 200.0f, 3);

    bulletType = NORMAL_BULLET;
    bulletSpeed = 1000.f;
    bulletDiameter = 20.f;
    lastShotTime = 1/playerInstance.fireRate;

    spawnDelay = 5.0f; // Give player some buffer
    currentWave = WAVE_0;
    isWaveStart = 1;
}

void level_one_update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    CP_Vector bgVec = CP_Vector_Set((LEVEL_WIDTH / 2), (LEVEL_HEIGHT / 2)),
        relBgVec = get_cam_pos_rel(bgVec, cam);
    CP_Image_Draw(levelBg, relBgVec.x, relBgVec.y, LEVEL_WIDTH, LEVEL_HEIGHT, 255);
    float dt = CP_System_GetDt();

    // Init enemy when wave start
    if (isWaveStart) {
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
            roaches[i] = init_enemy(LEVEL_HEIGHT * 1/5, LEVEL_HEIGHT * 4/5, enemySpeed, enemyMaxHealth, enemyDmg);
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
    } 
    else {
        for (int i = 0; i < roachesMax; i++) {
            // Enemy movement logic and attacking logic
            
            struct Circle radarRoach = {.pos = roaches[i].pos, .diameter = 5 * roaches[i].width};
            
            // Check enemy near with player
            if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) {
                
                // When collide...
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) {
                    // Stop movement of enemy
                    roaches[i].movement = STOPPED;

                    // Attack goes here...
                    roaches[i].currentState = STATE_ATTACKING;
                }
                else {
                    enemy_chase_player(&roaches[i], playerInstance);
                }
            }
            else {
                // priority to move to defense target
                enemy_move_to_tower(&roaches[i], towerInstance);
            }
            // Update position of enemy
            update_pos_enemy(&roaches[i], dt);
        }
    }

    lastShotTime += dt;
    if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
        if (lastShotTime >= (1 / playerInstance.fireRate)) {
            float mouse_x = CP_Input_GetMouseWorldX(),
                mouse_y = CP_Input_GetMouseWorldY();
            CP_Vector bulletDirection = CP_Vector_Subtract(CP_Vector_Set(mouse_x, mouse_y), get_cam_pos_rel(playerInstance.pos, cam));
            struct Bullet bullet = init_bullet(bulletType, bulletSpeed, bulletDiameter, 0, bulletDirection);
            shoot_bullet(&playerInstance, &bullet);
            lastShotTime = 0;
        }
    }
    
    move_player(&playerInstance, dt);
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Update hitbox of player
    render_player(playerInstance, 255, cam);

    for (int i = 0; i < sizeof(worldClips); i++) {
        struct Rectangle worldClip = worldClips[i];
        if (are_rects_intersecting(playerInstance.hitbox, worldClip))
            clamp_pos_clip_rel(&playerInstance.pos, playerInstance.hitbox, worldClip);
    }

    render_tower(towerInstance, cam);
    render_player(playerInstance, 255, cam);
    
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        update_pos_bullet(&playerInstance.bullets[i], dt);
        render_bullet(playerInstance.bullets[i], cam);
    }

    for (int i = 0; i < roachesMax; i++) {
        CP_Vector hitboxPos = { roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(roaches[i].width), roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(roaches[i].height) };
        
        update_rect_hitbox(&roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Update hitbox of enemy
        // Some code here...    // Sort enemy based on pos asc
        render_enemy(roaches[i], cam); // Draw enemy instance
    }

    // Render UI
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
}

void level_one_exit(void) {
    CP_Image_Free(&playerInstance.img);
    CP_Image_Free(&levelBg);

    for (int i = 0; i < roachesMax; i++) {
        struct ChocoRoach roach = roaches[i];
        if (roach.img != NULL)
            CP_Image_Free(&roach.img);
    }
}
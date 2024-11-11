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

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define MAX_ENEMY_COUNT 100

const float LEVEL_WIDTH = 2000.0f,
    LEVEL_HEIGHT = 900.0f;
CP_Image levelBg;

#pragma region Enemy Properties
// ************************************* //
// *         Spawn Properties          * //
// ************************************* //
float spawnMinY = 0.0f, spawnMaxY = 0.0f;
float spawnDelay = 0.0f, spawnTime = 0.0f;
enum SpawnWave {
    WAVE_0,
    WAVE_1,
    WAVE_2,
    WAVE_3
};
enum SpawnWave currentWave = WAVE_0;
char isWaveStart = 0;
// ************************************* //

unsigned short int roachesMax = 0;
struct ChocoRoach roaches[MAX_ENEMY_COUNT];
#pragma endregion

#pragma region Player Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
#pragma endregion

#pragma region Obstacle (Clips) Properties
struct Rectangle worldClips[1];
struct Rectangle playerClips[100];
struct Rectangle enemyClips[100];

struct Rectangle playerBarricades[10];
#pragma endregion

void level_one_init(void) {
    levelBg = CP_Image_Load("Assets/Levels/level1_placeholder.png");
    cam = init_camera(playerInstance, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.03, LEVEL_HEIGHT / 2);
    worldClips[0] = get_tower_hitbox(towerInstance);

    playerInstance = init_player(CP_Vector_Set(towerInstance.pos.x / 1.05, towerInstance.pos.y), 350.0f, 3);

    roachesMax = 40;
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
            enemySpeed = 200.f;
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
            roaches[i] = init_enemy(LEVEL_HEIGHT * 1/5, LEVEL_HEIGHT * 4/5, enemySpeed, enemyMaxHealth, enemyDmg);
        }
        isWaveStart = 0;
    }

    // Enemy spawning
    if (spawnTime < spawnDelay) {
        spawnTime += dt;
    } else {
        for (int i = 0; i < roachesMax; i++) {
            // Enemy movement logic and attacking logic
            
            struct Circle radarRoach = {.pos = roaches[i].pos, .diameter = 5 * roaches[i].width};
            
            // Check enemy near with player
            if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) {
                
                // When collide...
                if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) {
                    printf("Intersecting...\n");
                    // Stop movement of enemy
                    roaches[i].movement = STOPPED;

                    // Attack goes here...
                    roaches[i].currentState = STATE_ATTACKING;
                }
                else {
                    // Chase player
                    printf("Chasing...\n");
                    chase_player(&roaches[i], playerInstance);
                }
            }
            else {
                printf("Far away from player...\n");
                roaches[i].movement = MOVING_RIGHT;
            }

            // Update position of enemy
            update_pos_enemy(&roaches[i], dt);

        }
    }

    // Enemies movement logic
    for (int i = 0; i < roachesMax; i++) {

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
    
    move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT);
    // Move enemies here
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Player

    for (int i = 0; i < sizeof(worldClips); i++) {
        struct Rectangle worldClip = worldClips[i];
        if (are_rects_intersecting(playerInstance.hitbox, worldClip))
            clamp_pos_clip_rel(&playerInstance.pos, playerInstance.hitbox, worldClip);
    }

    render_tower(towerInstance, cam);
    render_player(playerInstance, 255, cam);
    
    for (int i = 0; i < roachesMax; i++) {
        CP_Vector hitboxPos = { roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(roaches[i].width), roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(roaches[i].height) };
        update_rect_hitbox(&roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Player
        // Draw enemy instance
        render_enemy(roaches[i], cam);
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
            if (are_circle_rect_intersecting(hitbox, playerInstance.hitbox)) {
                if (!player_take_damage(&playerInstance, 1)) {
                    if (!tower_revive_player(&towerInstance, &playerInstance)) {
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
}
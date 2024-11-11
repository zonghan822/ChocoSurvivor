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

const float LEVEL_WIDTH = 3000.0f,
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
// ************************************* //

unsigned short int roachesMax = 0;
struct ChocoRoach roaches[MAX_ENEMY_COUNT];
#pragma endregion

#pragma region Player Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
#pragma endregion

#pragma region Obstacle Properties
const struct Rectangle WORLD_OBSTACLES[100];
struct Rectangle playerBarricades[10];
#pragma endregion

void level_one_init(void) {
    levelBg = CP_Image_Load("Assets/Levels/level1_placeholder.png");
    cam = init_camera(playerInstance, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

    playerInstance = init_player(CP_Vector_Set(300.0f, 500.0f), 155.0f, 3);

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.03, LEVEL_HEIGHT / 2);

    roachesMax = 40;
    spawnDelay = 10.0f; // Give player some buffer

    // Define enemy spawn area
    spawnMinY = LEVEL_HEIGHT / 10.0;
    spawnMaxY = LEVEL_HEIGHT * 9.0 / 10.0;
}

void level_one_update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    CP_Vector bgVec = CP_Vector_Set((LEVEL_WIDTH / 2), (LEVEL_HEIGHT / 2)),
        relBgVec = get_cam_pos_rel(bgVec, cam);
    CP_Image_Draw(levelBg, relBgVec.x, relBgVec.y, LEVEL_WIDTH, LEVEL_HEIGHT, 255);
    float dt = CP_System_GetDt();
    
    if (spawnTime < spawnDelay) {
        spawnTime += dt;
    }

    if (spawnTime >= spawnDelay) {
        // PLACEHOLDER
        spawnDelay = 1.3f;

        srand(time(NULL));
        for (int i = 0; i < roachesMax; i++) {
            if (roaches[i].health > 0) continue;
            struct ChocoRoach chocoRoach = {
                .img = NULL,
                .pos.x = -35.f,
                .pos.y = rand_float(spawnMinY, spawnMaxY), // Generate random y pos for enemies
                .radius = 35.f,
                .degree = 0.f,
                .speed = 90.f,
                .health = 100,
                .dmg = 10,
                .movement = MOVING_RIGHT,
                .current_state = STATE_MOVING
            };

            // Store enemies in array
            roaches[i] = chocoRoach;
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
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Player

    render_tower(towerInstance, cam);
    render_player(playerInstance, 255, cam);
    render_enemies(roaches, roachesMax, cam);

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

        // TODO: Add one for boss

        // Tower collision with player (impassable)
        if (are_rects_intersecting(playerInstance.hitbox, get_tower_hitbox(towerInstance)))
        {
            // Clamp player pos?
            // clamp_player_pos(struct Rectangle* entity_hitbox, struct Rectangle obstacle);
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
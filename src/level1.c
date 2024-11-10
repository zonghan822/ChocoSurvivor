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

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define MAX_ENEMY_COUNT 100

const float LEVEL_WIDTH = 2000.0f,
    LEVEL_HEIGHT = 1200.0f; 

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

struct ChocoRoach roaches[MAX_ENEMY_COUNT];
enum SpawnWave currentWave = WAVE_0;
#pragma endregion

#pragma region Player Properties
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;
#pragma endregion

void level_one_init(void) {
    cam = init_camera(playerInstance, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

    playerInstance = init_player(CP_Vector_Set(300, 500), 350.0f, 3);

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.1, LEVEL_HEIGHT / 2);

    roachesMax = 40;
    spawnDelay = 5.0f; // Give player some buffer

    currentWave = WAVE_0;
    isWaveStart = 1;
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
    
    move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Player

    render_tower(towerInstance, cam);
    render_player(playerInstance, 255, cam);

    for (int i = 0; i < roachesMax; i++) {
        CP_Vector hitboxPos = { roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(roaches[i].width), roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(roaches[i].height) };
        update_rect_hitbox(&roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Player
        // Draw enemy instance
        render_enemy(roaches[i], cam);
    }

    printf("%f, %f\n", playerInstance.pos.x, playerInstance.pos.y);
    printf("Tower: %f, %f | Should render when: %f, %f\n", towerInstance.pos.x, towerInstance.pos.y, towerInstance.pos.x - cam.pos.x, towerInstance.pos.y - cam.pos.y);

}

void level_one_exit(void) {

}
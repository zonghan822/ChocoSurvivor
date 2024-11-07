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

const float LEVEL_WIDTH = 3000.0f,
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

    playerInstance = init_player(CP_Vector_Set(300, 500), 140.0f, 3);

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.1, LEVEL_HEIGHT / 2);

    roachesMax = 40;
    spawnDelay = 10.0f; // Give player some buffer

    // Define enemy spawn area
    spawnMinY = (float)CP_System_GetWindowHeight() / 10.0;
    spawnMaxY = (float)CP_System_GetWindowWidth() * 9.0 / 10.0;
}

void level_one_update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    float dt = CP_System_GetDt();
    
    // Enemy spawning not working?
    /*if (spawnTime < spawnDelay) {
        spawnTime += dt;
    }

    if (spawnTime >= spawnDelay) {
        // PLACEHOLDER
        spawnDelay = 1.3f;

        srand(time(NULL));
        for (int i = 0; i < roachesMax; i++) {
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
    }*/
    
    move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Player

    render_tower(towerInstance, cam);
    render_player(playerInstance, 255, cam);
    render_enemies(roaches, roachesMax);

    printf("%f, %f\n", playerInstance.pos.x, playerInstance.pos.y);
    printf("Tower: %f, %f | Should render when: %f, %f\n", towerInstance.pos.x, towerInstance.pos.y, towerInstance.pos.x - cam.pos.x, towerInstance.pos.y - cam.pos.y);

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
}

void level_one_exit(void) {

}
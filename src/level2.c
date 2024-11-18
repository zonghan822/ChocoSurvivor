#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#include "level2.h"
#include "boss.h"
#include "player.h"
#include "tower.h"
#include "cprocessing.h"
#include "camera.h"
#include "hittest.h"
#include "enemy.h"
#include "utils.h"

#define ARRAY_COUNT(x) (sizeof(x)/sizeof(x[0]))
#define MAX_ENEMY_COUNT 100
#define LEVEL_WIDTH 3000.0f
#define LEVEL_HEIGHT 900.0f

CP_Image levelBg,
    cursor, gameOverImage;

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
struct ChocoRoach roaches[MAX_ENEMY_COUNT];
struct Boss bossInstance;

// ************************************* //
// *        Player Properties          * //
// ************************************* //
struct ChocoShooter playerInstance;
struct DefenseTarget towerInstance;
struct Camera cam;

// ************************************* //
// *         Obstacles / Clips         * //
// ************************************* //
struct Rectangle worldClips[1];
struct Rectangle playerClips[1];
struct Rectangle enemyClips[1];
struct Rectangle playerBarricades[10];

void level_two_init(void) {
    cursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);
    levelBg = CP_Image_Load("Assets/Levels/level2.png");
    gameOverImage = CP_Image_Load("Assets/gg.png");
    cam = init_camera(playerInstance, (float)CP_System_GetWindowWidth(), (float)CP_System_GetWindowHeight());

    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.03, LEVEL_HEIGHT / 2.3);
    worldClips[0] = get_tower_hitbox(towerInstance);

    playerInstance = init_player(CP_Vector_Set(towerInstance.pos.x / 1.05, towerInstance.pos.y), 350.0f, 100, 30, 10.f);
    bossInstance = init_boss_two(400.f, 95.f, 600, 100, 3.f);

    roachesMax = 40;
    spawnDelay = 5.0f; // Give player some buffer

    currentWave = WAVE_0;
    isWaveStart = 1;
}

void level_two_update(void) {
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    CP_Vector bgVec = CP_Vector_Set((LEVEL_WIDTH / 2), (LEVEL_HEIGHT / 2)),
        relBgVec = get_cam_pos_rel(bgVec, cam);
    CP_Image_Draw(levelBg, relBgVec.x, relBgVec.y, LEVEL_WIDTH, LEVEL_HEIGHT, 255);
    float dt = CP_System_GetDt();
    float mouseX = CP_Input_GetMouseWorldX(),
        mouseY = CP_Input_GetMouseWorldY();

    CP_Vector mouseVec = get_world_pos_rel(CP_Vector_Set(mouseX, 0.f), cam);
    if (playerInstance.pos.x > mouseVec.x)
        playerInstance.width = playerInstance.width > 0 ? -playerInstance.width : playerInstance.width;
    else
        playerInstance.width = playerInstance.width < 0 ? -playerInstance.width : playerInstance.width;

    move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT);
    bossInstance.move(&bossInstance, playerInstance.pos, dt);
    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Player

    for (int i = 0; i < sizeof(worldClips); i++) {
        struct Rectangle worldClip = worldClips[i];
        if (are_rects_intersecting(playerInstance.hitbox, worldClip))
            clamp_pos_clip_rel(&playerInstance.pos, playerInstance.hitbox, worldClip);
    }

    render_tower(towerInstance, cam);
    render_boss(bossInstance, cam);
    render_player(playerInstance, 255, cam);

    if (bossInstance.canSecondaryAttack(bossInstance, playerInstance))
        bossInstance.secondaryAttack(&bossInstance, &playerInstance, dt);
    if (bossInstance.canPrimaryAttack(bossInstance, playerInstance))
        bossInstance.primaryAttack(&bossInstance, &playerInstance, dt);

    if (towerInstance.health <= 0) {
        // Draw the Game Over image in the center of the screen
        float gameOverX = CP_System_GetWindowWidth() / 2.f,
            gameOverY = CP_System_GetWindowHeight() / 2.f;
        CP_Image_Draw(gameOverImage, gameOverX, gameOverY, 1000, 600, 255); // Adjust size as needed
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

    CP_Image_Draw(cursor, mouseX, mouseY, 70, 50, 255);
}

void level_two_exit(void) {
    CP_Image_Free(&playerInstance.img);
    CP_Image_Free(&levelBg);
    CP_Image_Free(&cursor);
    CP_Image_Free(&gameOverImage);
    CP_Image_Free(&bossInstance.img);

    for (int i = 0; i < roachesMax; i++) {
        struct ChocoRoach roach = roaches[i];
        if (roach.img != NULL)
            CP_Image_Free(&roach.img);
    }
}
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
#include "boss.h"
#include "button.h"
#include "audio.h"

#define MAX_ENEMY_COUNT 100
#define LEVEL_WIDTH 2400.0f
#define LEVEL_HEIGHT 900.0f

CP_Image levelBg, cursor;

float windowWidth, windowHeight;

#pragma region Spawn Properties
static float spawnMinY = 0.f, spawnMaxY = 0.f, spawnWidth = 0.f;
static float spawnInterval = 0.0f, spawnTimer = 0.f; // Spawn interval
static float waveDelay = 0.f, waveTimer = 0.f; // Wave delay
static int enemySpawnIndex = 0;
static enum SpawnWave {
    WAVE_1 = 1,
    WAVE_2 = 2,
    WAVE_3 = 3
};
static enum SpawnWave currentWave = WAVE_1;
#pragma endregion

#pragma region Enemy Properties
static enum AttackTarget {
    NONE,
    PLAYER,
    TOWER,
    BARRIER
};
static enum AttackTarget enemyAttackTarget = NONE;
static unsigned short int roachesMax = 0;
static struct ChocoRoach roaches[MAX_ENEMY_COUNT];
static float healthBarDisplayTime = 0.f;
static int deadEnemies = 0;
static struct Boss bossInstance;
#pragma endregion

#pragma region Player_Properties
static struct DefenseTarget towerInstance;
static struct Camera cam;
static enum BulletType bulletType = NORMAL_BULLET;
static float bulletSpeed = 650.f, bulletDiameter = 14.f;
static float lastShotTime = 0;
static bool enableShooting = false;
static CP_Vector respawnVec;
#pragma endregion

#pragma region HP/BUFF bar properties
static float meterX = 0;
static float meterY = 0;
static float meterWidth = 0;
static float meterHeight = 0;
static float fillSpeed = 0.0f;
static float fillWidth = 0;
static float targetFillWidth = 0;
static int activeBuffSlot = 0;          // 0 means no active buff, 1-3 represent the buff slots
//float activeBuffDuration = 0.0f;  // Duration in seconds for the active buff (10s when active)
#pragma endregion

#pragma region Obstacle/Barrier Properties
static struct Rectangle playerBarricades[10];
static enum BarrierState currentState = BARRIER_IDLE;
static CP_Vector previewPosition;
static CP_Vector finalPosition;
static float barrierWidth = 0.0f;
static float barrierHeight = 0.0f;
static float barrier_rect_W = 0.0f, barrier_rect_H = 0.0f;
#pragma endregion

#pragma region Map Properties
static struct Rectangle worldClips[100];
static struct Rectangle playerClips[1];
static struct Rectangle enemyClips[1];
static int worldClipsCount = 0;
static PathNode grid[MAX_NODE_SIZE];
static int nodeCount = 0;
#pragma endregion

static struct Button menuButton;

static void _render_cursor(float mouse_x, float mouse_y) {
    CP_Settings_ImageMode(CP_POSITION_CORNER);
    float cursorWidth = 60.f;
    // 20.f is to correct its draw position with the actual cursor position
    CP_Image_Draw(cursor, mouse_x - 20.f, mouse_y, cursorWidth,
        get_preserved_heightf(CP_Image_GetWidth(cursor), CP_Image_GetHeight(cursor), cursorWidth), 255);
    CP_Settings_ImageMode(CP_POSITION_CENTER);
}

static int _get_exp(int health, float speed, int max_enemies) {
    return (int)((float)(health / speed) * (float)(max_enemies / 10.f));
}

void level_two_init(void) {
    enableShooting = false;
    cursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);
    levelBg = CP_Image_Load("Assets/Levels/level2.png");

    windowWidth = (float)CP_System_GetWindowWidth();
    windowHeight = (float)CP_System_GetWindowHeight();

    bossInstance = init_boss_two(375.f, 65.f, 12500, 100, 1.1f);

    #pragma region Defense_Target_Init
    // Init Defense Target Properties
    towerInstance.maxHealth = towerInstance.health = 3;
    towerInstance.height = 300.0;
    towerInstance.width = 150.0;
    towerInstance.pos = CP_Vector_Set(LEVEL_WIDTH / 1.25f, LEVEL_HEIGHT / 1.55f);
    towerInstance.flickerState.isVisible = TRUE;
    #pragma endregion

    #pragma region Player_Init
    // Init Player Properties
    respawnVec = CP_Vector_Set(towerInstance.pos.x / 1.15f, towerInstance.pos.y);
    playerInstance.pos = respawnVec;
    playerInstance.flickerState.isVisible = TRUE; // set initial flickering states
    bulletType = NORMAL_BULLET;
    #pragma endregion

    #pragma region Barrier_Init
    // Init Barrier Properties
    currentState = BARRIER_IDLE;
    barrierWidth = 50.0f;
    barrierHeight = 100.0f;
    barrier_rect_W = 50.0f;
    barrier_rect_H = 50.0f;
    #pragma endregion

    #pragma region Enemy_Spawn_Init
    memset(roaches, 0, sizeof(roaches));
    // Reset spawn properties
    waveDelay = 5.0f; // Give player some buffer
    currentWave = WAVE_1;
    enemySpawnIndex = 0;
    waveTimer = 0.f;
    spawnTimer = 0.f;
    deadEnemies = 0;
    spawnMinY = LEVEL_HEIGHT * 1 / 5;
    spawnMaxY = LEVEL_HEIGHT * 4 / 5;
    spawnWidth = 300.f;

    meterX = windowWidth - 150;//  buff bar properties
    meterY = windowHeight - 50;
    meterWidth = 100;
    meterHeight = 20;
    fillSpeed = 120.0f;
    #pragma endregion

    // enemy clips
    struct Rectangle ememyClip1 = { .pos = CP_Vector_Set(883.5f, 577.f), .width = 303.f, .height = 466.f };
    enemyClips[0] = ememyClip1;

    // store non-movable path in worldClips
    worldClips[0] = get_tower_hitbox(towerInstance);
    struct Rectangle holeClip = { .pos = CP_Vector_Set(1072.f, 171.5f), .width = 2144.f, .height = 31.f };
    struct Rectangle fountainClip = { .pos = CP_Vector_Set(1369.5f, 383.5f), .width = 253.f, .height = 361.f };
    worldClips[1] = holeClip;
    worldClips[2] = fountainClip;
    worldClipsCount = 3;

    cam = init_camera(playerInstance, windowWidth, windowHeight);
    nodeCount = init_grid(grid, LEVEL_WIDTH, LEVEL_HEIGHT, 75, 100);
    update_grid(grid, worldClips, worldClipsCount, enemyClips, 1);

    menuButton = init_button_image(windowWidth / 2.f, windowHeight / 1.25f, 250.f, 75.f,
        BUTTON_TYPE_DEFAULT, "Back to Menu", CP_Color_Create(0, 0, 0, 255), 45.f);

    init_sounds();
}

void level_two_update(void) {
    float dt = CP_System_GetDt();
    float mouseX = CP_Input_GetMouseWorldX(),
        mouseY = CP_Input_GetMouseWorldY();

    // PH
    // Handler for game over
    if (towerInstance.health <= 0) {
        CP_Graphics_ClearBackground(CP_Color_Create(15, 15, 15, 255));
        CP_Settings_TextSize(175.f);
        CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
        CP_Font_DrawText("GAME OVER", windowWidth / 2.f, windowHeight / 2.4f);
        render_button(menuButton);
        update_button_state(&menuButton, mouseX, mouseY);
        _render_cursor(mouseX, mouseY);

        if (menuButton.state == BUTTON_STATE_CLICKED)
            CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
        return;
    }

    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));

    CP_Vector bgVec = CP_Vector_Set((LEVEL_WIDTH / 2), (LEVEL_HEIGHT / 2)),
        relBgVec = get_cam_pos_rel(bgVec, cam);
    CP_Image_Draw(levelBg, relBgVec.x, relBgVec.y, LEVEL_WIDTH, LEVEL_HEIGHT, 255);

    enableShooting = true;

    CP_Vector mouseVec = get_world_pos_rel(CP_Vector_Set(mouseX, 0.f), cam);
    if (playerInstance.pos.x > mouseVec.x)
        playerInstance.width = playerInstance.width > 0 ? -playerInstance.width : playerInstance.width;
    else
        playerInstance.width = playerInstance.width < 0 ? -playerInstance.width : playerInstance.width;

    draw_grid(grid, nodeCount, cam); // draw grid for debugging

    // Enemy spawner when new wave
    if (waveTimer >= waveDelay) {

        if (spawnInterval > spawnTimer)
            spawnTimer += dt;
        else {
            float enemySpeed = 0.f;
            int enemyMaxHealth = 0, enemyDmg = 0;
            float nextSpawnDelay = 0.f;
            int nextSpawnDelayIndex = 0;
            switch (currentWave) {
            case WAVE_1:
                spawnInterval = 1.65f;
                enemySpeed = 90.f;
                enemyMaxHealth = 175;
                enemyDmg = 7;
                roachesMax = 35;
                nextSpawnDelayIndex = 6;
                nextSpawnDelay = -0.05f;
                break;
            case WAVE_2:
                spawnInterval = 1.35f;
                enemySpeed = 105.f;
                enemyMaxHealth = 250;
                enemyDmg = 10;
                roachesMax = 55;
                nextSpawnDelayIndex = 6;
                nextSpawnDelay = -0.15f;
                break;
            case WAVE_3:
                if (bossInstance.state == BOSS_STATE_UNSPAWNED)
                    spawn_boss(&bossInstance, -650.f, 650.f);
                spawnInterval = 1.65f;
                enemySpeed = 75.f;
                enemyMaxHealth = 300;
                enemyDmg = 15;
                roachesMax = 45;
                nextSpawnDelayIndex = 6;
                nextSpawnDelay = -0.55f;
                break;
            }

            if (enemySpawnIndex < roachesMax) {
                roaches[enemySpawnIndex++] = init_enemy(spawnMinY, spawnMaxY, spawnWidth, enemySpeed, enemyMaxHealth, enemyDmg);
                spawnTimer = 0.f;
                printf("Enemy spawned. Spawn count: %d | Remaining: %d\n", enemySpawnIndex, enemySpawnIndex - deadEnemies);

                // Increase delay a bit to not overwhelm the player
                if (enemySpawnIndex % nextSpawnDelayIndex == 0)
                    spawnTimer = nextSpawnDelay;
            }
        }
    }

    // Execute code if starting a new wave
    if (waveTimer < waveDelay) {
        char buffer[256];
        sprintf_s(buffer, sizeof(buffer), "Enemy coming in %d !!!", (int)(waveDelay - waveTimer));
        CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
        CP_Settings_TextSize(100.0f);
        CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
        CP_Font_DrawText(buffer, (float)CP_System_GetWindowWidth() / 2, (float)CP_System_GetWindowHeight() * 1 / 5);
        enemySpawnIndex = 0;
        waveTimer += dt;
    }
    else {
        for (int i = 0; roaches[i].currentState; i++) {
            // Attack defense target when collide
            if (are_rects_intersecting(roaches[i].hitbox, get_tower_hitbox(towerInstance))) {
                roaches[i].currentState = ENEMY_STATE_ATTACKING;
                enemyAttackTarget = TOWER;
                continue;
            }
            // Attack player when collide
            if (are_rects_intersecting(roaches[i].hitbox, playerInstance.hitbox)) { // When collide attack player
                roaches[i].currentState = ENEMY_STATE_ATTACKING;
                enemyAttackTarget = PLAYER;
                continue;
            }

            //if (barrierAttack) {
            //    roaches[i].currentState = STATE_ATTACKING;
            //    enemyAttackTarget = BARRIER;
            //}

            struct Circle radarRoach = { .pos = roaches[i].pos, .diameter = 5 * roaches[i].width }; // define enemy radar
            if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) { // Check enemy near with player
                roaches[i].currentState = ENEMY_STATE_MOVING_TO_PLAYER;
                continue;
            }

            roaches[i].currentState = ENEMY_STATE_MOVING_TO_TOWER;
        }
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
                if (are_circle_rect_intersecting(bullet, worldClip)) { // bullet cannot go through walls and barricades...
                    playerInstance.bullets[j].isShooting = FALSE;
                }
            }
        }
    }

    update_camera(&cam, playerInstance, LEVEL_WIDTH, LEVEL_HEIGHT);

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
        previewPosition = get_world_pos_rel(CP_Vector_Set(mouseX, mouseY), cam);

        // Draw translucent preview rectangle
        CP_Settings_RectMode(CP_POSITION_CENTER);
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 128)); // Semi-transparent blue
        CP_Graphics_DrawRect(mouseX, mouseY, barrierWidth, barrierHeight);

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
                worldClips[worldClipsCount++] = barriers[barrierIndex].hitbox;
                update_grid(grid, worldClips, worldClipsCount, enemyClips, 1);
            }
        }
        break;
    }

    // Damage the first active barrier when pressing 'B'
    if (CP_Input_KeyTriggered(KEY_B)) {
        barrier_take_damage(1);
    }

    lastShotTime += dt;
    if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT) && enableShooting) {
        if (lastShotTime >= (1 / playerInstance.attackSpeed)) { // control fire rate
            // bullet direction is the vector of player with mouse pointer
            CP_Vector bulletDirection = CP_Vector_Subtract(CP_Vector_Set(mouseX, mouseY), get_cam_pos_rel(playerInstance.pos, cam));
            struct Bullet bullet = init_bullet(bulletType, bulletSpeed, bulletDiameter, playerInstance.attackPower, bulletDirection);
            shoot_bullet(&playerInstance, &bullet);
            lastShotTime = 0;
        }
    }

    // Render Defense Target
    update_flicker(&towerInstance.flickerState, dt);
    if (towerInstance.flickerState.isVisible)
        render_tower(towerInstance, cam);

    // Update and render player
    update_flicker(&playerInstance.flickerState, dt);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Update hitbox of player
    if (playerInstance.flickerState.isVisible)
        render_player(playerInstance, 255, cam);
    render_boss(bossInstance, cam);

    // Prevent player and bullets move into non-movable path
    for (int i = 0; i < worldClipsCount; i++) {
        struct Rectangle worldClip = worldClips[i];
        if (are_rects_intersecting(playerInstance.hitbox, worldClip))
            clamp_pos_clip_rel(&playerInstance.pos, playerInstance.hitbox, worldClip);
        for (int j = 0; j < MAX_BULLET_COUNT; j++) {
            if (playerInstance.bullets[j].isShooting) {
                struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
                if (are_circle_rect_intersecting(bullet, worldClip)) { // bullet cannot go through walls and barricades...
                    playerInstance.bullets[j].isShooting = FALSE;
                }
            }
        }
    }

    // Draw all placed barriers
    struct Barrier* barriers = get_barriers();
    CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255)); // Solid blue
    for (int i = 0; i < get_barrier_count(); i++) {
        if (barriers[i].health > 0) {  // Only draw if health is greater than zero
            CP_Vector vec = get_cam_pos_rel(barriers[i].position, cam);
            CP_Graphics_DrawRect(vec.x, vec.y, barriers[i].width, barriers[i].height);
        }
    }

    // Update and render bullets
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        update_pos_bullet(&playerInstance.bullets[i], dt);
        render_bullet(playerInstance.bullets[i], cam);
    }

    // Boss movement and attack logic
    switch (bossInstance.state) {
    case BOSS_STATE_MOVING_TO_TOWER:
        bossInstance.move(&bossInstance, playerInstance, get_tower_hitbox(towerInstance), dt, grid, nodeCount);
        break;

    case BOSS_STATE_MOVING_TO_PLAYER:
        bossInstance.move(&bossInstance, playerInstance, playerInstance.hitbox, dt, grid, nodeCount);
        break;

    case BOSS_STATE_IDLE:
        bossInstance.moveDirection = CP_Vector_Zero();
        break;
    }

    if (bossInstance.canPrimaryAttack(bossInstance, playerInstance))
        bossInstance.primaryAttack(&bossInstance, &playerInstance, dt);
    else if (bossInstance.canSecondaryAttack(bossInstance, playerInstance))
        bossInstance.secondaryAttack(&bossInstance, &playerInstance, dt);

    for (int i = 0; i < roachesMax; i++) {
        if (roaches[i].currentState) { // if enemy is alive

            switch (roaches[i].currentState) {
            case ENEMY_STATE_MOVING_TO_TOWER: {
                enemy_move_to_target(&roaches[i], get_tower_hitbox(towerInstance), dt, grid, nodeCount);
                break;
            }

            case ENEMY_STATE_MOVING_TO_PLAYER: {
                enemy_move_to_target(&roaches[i], playerInstance.hitbox, dt, grid, nodeCount);
                break;
            }

            case ENEMY_STATE_ATTACKING: {
                // stop movement
                roaches[i].moveDirection = CP_Vector_Zero();

                // attack animation here...


                // if animation end then...
                switch (enemyAttackTarget) {
                case PLAYER:
                    if (!player_take_damage(&playerInstance, roaches[i].dmg)) { // Returns 0 if player dies from damage
                        if (tower_revive_player(&towerInstance, &playerInstance)) // Returns 1 if tower is still alive after reviving
                            playerInstance.pos = respawnVec;
                    }
                    break;
                case TOWER:
                    tower_take_damage(&towerInstance, 1);
                    break;
                case BARRIER:
                    //barrierTakeDamage...
                    break;
                }
                break;
            }

            case ENEMY_STATE_IDLE:
                roaches[i].moveDirection = CP_Vector_Zero();
                break;
            }

            CP_Vector hitboxPos = { roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(roaches[i].width), roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(roaches[i].height) };
            update_rect_hitbox(&roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Update hitbox of enemy

            for (int j = 0; j < MAX_BULLET_COUNT; j++) {
                if (playerInstance.bullets[j].isShooting) {
                    struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
                    if (are_circle_rect_intersecting(bullet, roaches[i].hitbox)) {
                        if (enemy_take_damage(&roaches[i], playerInstance.bullets[j].dmg)) {
                            deadEnemies++;
                            // Base exp on this formula:
                            // (max health / speed) * (max enemies / 10)
                            int expGained = _get_exp(roaches[i].maxHealth, roaches[i].speed, roachesMax);
                            enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained);

                            // set element to 0
                            memset(&roaches[i], 0, sizeof(roaches[i]));

                            // Apply the buff if one was granted
                            if (randomBuff != 0) {
                                apply_buff(&playerInstance, randomBuff);
                            }

                            // New wave
                            if (deadEnemies == roachesMax) {
                                if (currentWave < WAVE_3) {
                                    deadEnemies = 0; // Reset
                                    currentWave++;
                                    waveTimer = 0.f; // Reset wave timer to start next wave
                                }
                                else {
                                    // next level or something
                                }
                            }
                        }
                        playerInstance.bullets[j].isShooting = FALSE;
                    }
                }
            }
        }
        // can use else statement here for dying animation...
    }

    for (int j = 0; j < MAX_BULLET_COUNT; j++) {
        if (playerInstance.bullets[j].isShooting) {
            struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
            if (are_circle_rect_intersecting(bullet, bossInstance.hitbox)) {
                if (boss_take_damage(&bossInstance, playerInstance.bullets[j].dmg)) {
                    float expGained = _get_exp(bossInstance.maxHealth, bossInstance.speed, 1);
                    enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained * 12.f);

                    // Apply the buff if one was granted
                    if (randomBuff != 0) {
                        apply_buff(&playerInstance, randomBuff);
                    }
                    dispose_boss(&bossInstance);
                }
                playerInstance.bullets[j].isShooting = FALSE;
            }
        }
    }

    // Buff and currency drops here...

    // Draw the empty part of the meter (white)
    CP_Settings_RectMode(CP_POSITION_CORNER);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    CP_Graphics_DrawRect(meterX, meterY, meterWidth, meterHeight);
    if (fillWidth >= meterWidth) {
        CP_Settings_Fill(CP_Color_Create(200, 0, 255, 255)); // purple for maxed-out meter
    }
    else {
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 255)); // Blue for normal meter
    }
    CP_Graphics_DrawRect(meterX, meterY, fillWidth, meterHeight);
    CP_Settings_TextSize(20.0f);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    // buff slot 1 
    CP_Graphics_DrawCircle(150, 825, 80);
    //buff slot 2
    CP_Graphics_DrawCircle(300, 825, 80);
    // buff slot 3
    CP_Graphics_DrawCircle(450, 825, 80);

    // Display the active buff slot
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
    CP_Graphics_DrawCircle(600, 825, 100);

    for (int i = 0; i < 3; i++) {
        // Set the position of each buff slot circle
        CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));

        float slotPositions[] = { 150, 300, 450 }; // Positions for buff slots

        for (int i = 0; i < 3; i++) {
            switch (playerInstance.buffs.type[i]) {
            case ATTACK_UP:
                CP_Font_DrawText(
                    playerInstance.buffs.attackPowerLevel > 3 ? "A Max Lvl" :
                    playerInstance.buffs.attackPowerLevel == 1 ? "A Lvl 1" :
                    playerInstance.buffs.attackPowerLevel == 2 ? "A Lvl 2" : "A Lvl 3",
                    slotPositions[i], 825);
                break;
            case ATTACK_SPEED_UP:
                CP_Font_DrawText(
                    playerInstance.buffs.attackSpeedLevel > 3 ? "AS Max Lvl" :
                    playerInstance.buffs.attackSpeedLevel == 1 ? "AS Lvl 1" :
                    playerInstance.buffs.attackSpeedLevel == 2 ? "AS Lvl 2" : "AS Lvl 3",
                    slotPositions[i], 825);
                break;
            case HEALTH_UP:
                CP_Font_DrawText(
                    playerInstance.buffs.healthLevel > 3 ? "HP Max Lvl" :
                    playerInstance.buffs.healthLevel == 1 ? "HP Lvl 1" :
                    playerInstance.buffs.healthLevel == 2 ? "HP Lvl 2" : "HP Lvl 3",
                    slotPositions[i], 825);
                break;
            default:
                CP_Font_DrawText("Empty", slotPositions[i], 825);
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
    // Set targetFillWidth based on updated exp
    targetFillWidth = (playerInstance.exp / (float)playerMaxExp) * meterWidth;
    fillWidth = CP_Math_ClampFloat(fillWidth, 0, meterWidth); // Clamp to the actual visual width

    if (fillWidth < targetFillWidth) {
        fillWidth += fillSpeed * dt; // Use delta time for smooth updates
        if (fillWidth >= targetFillWidth) fillWidth = targetFillWidth; // Avoid overshooting
    }
    else if (fillWidth > targetFillWidth) {
        fillWidth -= fillSpeed * dt;
        if (fillWidth < targetFillWidth) fillWidth = targetFillWidth;
    }
    CP_Settings_RectMode(CP_POSITION_CENTER);

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
        sprintf_s(currentWaveStr, sizeof(currentWaveStr), "Wave: %d", currentWave);
        CP_Font_DrawText(currentWaveStr, windowWidth / 2.f, 50.f);
    }
#pragma endregion

    // Exit to Main Menu
    if (CP_Input_KeyTriggered(KEY_Q)) {
        CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
    }

    _render_cursor(mouseX, mouseY);
}

void level_two_exit(void) {
    CP_Image_Free(&playerInstance.img);
    CP_Image_Free(&levelBg);
    CP_Image_Free(&cursor);

    for (int i = 0; i < roachesMax; i++) {
        struct ChocoRoach roach = roaches[i];
        if (roach.img != NULL)
            CP_Image_Free(&roach.img);
    }
    if (bossInstance.img != NULL)
        CP_Image_Free(&bossInstance.img);
    dispose_sounds();
    dispose_button(&menuButton);
}
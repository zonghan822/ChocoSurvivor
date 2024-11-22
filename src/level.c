#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "cprocessing.h"
#include "hittest.h"
#include "player.h"
#include "level.h"
#include "progressbar.h"
#include "boss.h"
#include "enemy.h"
#include "placeables.h"
#include "camera.h"
#include "button.h"
#include "level2.h"
#include "mainmenu.h"
#include "audio.h"
#include "utils.h"

// Macros
#define MAX_ENEMY_COUNT 100

#pragma region Spawn Properties
static float _spawnMinY = 0.f, _spawnMaxY = 0.f, _spawnWidth = 0.f;
static float _spawnInterval = 0.0f, _spawnTimer = 0.f;      // Spawn interval
static float _waveDelay = 0.f, _waveTimer = 0.f;            // Wave delay
static int _enemySpawnIndex = 0;
static int _totalEnemies = 0;
#pragma endregion

#pragma region Enemy Properties
static enum AttackTarget {
    TARGET_NONE,
    TARGET_PLAYER,
    TARGET_TOWER,
    TARGET_BARRIER
};
static enum AttackTarget _enemyAttackTarget = TARGET_NONE;
static unsigned short int _roachesMax = 0;
static struct ChocoRoach _roaches[MAX_ENEMY_COUNT];
static int _deadEnemies = 0;
static struct Boss _boss;
static float _enemyMaxAttackCD = 0.f;
#pragma endregion

#pragma region Player_Properties
static struct DefenseTarget _tower;
bool enableShooting = false; // Global, not private
static CP_Vector _respawnVec;
#pragma endregion

#pragma region Barrier Properties
static struct Rectangle _playerBarriers[10];
static enum BarrierState _currentBarrierState = BARRIER_IDLE;
static CP_Vector _previewPosition;
static CP_Vector _finalPosition;
#pragma endregion

#pragma region Map Properties
static struct Rectangle _worldClips[100];
static struct Rectangle _playerClips[1];
static struct Rectangle _enemyClips[1];
static int _obstaclesCount = 0;
static int _worldClipsCount = 0;
static Grid _enemyGrid, _bossGrid, _enemyGridNB, _bossGridNB;
#pragma endregion

#pragma region Ally Properties
static enum AllyState _currentAllyState = ALLY_IDLE;
#pragma endregion

#pragma region HP/BUFF bar properties
static int activeBuffSlot = 0;          // 0 means no active buff, 1-3 represent the buff slots
//static float activeBuffDuration = 0.0f;  // Duration in seconds for the active buff (10s when active)
#pragma endregion

enum SpawnWave currentWave;

static struct Button _menuButton;
static bool _isMovingToNextLevel;
static bool _hasPlayedGameOverMusic,
    _hasPlayedNewWaveMusic;
static enum GameLevel _currentLevel;
static float _levelWidth, _levelHeight;
static CP_Image _heartImg;

// Naming convention: No prefix if level-related
//                    player_xyz if player-related
//                    render_xyz if render-related
// etc

static void _draw_shadows(float x, float y, float width, float height) {
    CP_Settings_StrokeWeight(0.f);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 105));
    CP_Graphics_DrawEllipse(x, y + (height / 2.f), width, height / 3.5f);

    // Reset
    CP_Settings_StrokeWeight(3.f);
}

static int _get_exp(int health, float speed, int max_enemies) {
    return (int)((float)(health / speed) * (float)(max_enemies / 10.f));
}

// Returns an instance of the level background
CP_Image init_level(enum GameLevel level, float level_width, float level_height, float window_width, float window_height) {
    _currentLevel = level;
    currentWave = WAVE_1;
    _levelWidth = level_width;
    _levelHeight = level_height;
    _heartImg = CP_Image_Load("Assets/heart.png");

    // Reset values
    _isMovingToNextLevel = false;
    _hasPlayedGameOverMusic = false;
    _hasPlayedNewWaveMusic = false;
    enableShooting = false;
    _enemyAttackTarget = 0;
    _previewPosition = CP_Vector_Zero();
    _finalPosition = CP_Vector_Zero();
    _currentBarrierState = BARRIER_IDLE;
    _obstaclesCount = 0;
    _worldClipsCount = 0;
    _waveDelay = 10.f;
    _enemySpawnIndex = 0;
    _waveTimer = 0.f;
    _spawnTimer = 0.f;
    _deadEnemies = 0;

    // Can move to level specific cases
    _spawnMinY = level_height * 1 / 5;
    _spawnMaxY = level_height * 4 / 5;
    _spawnWidth = 300.f;

    CP_Image enemyImg = CP_Image_Load("Assets/choco_roach.png");
    float enemyHeight = get_preserved_heightf((float)CP_Image_GetWidth(enemyImg),
        (float)CP_Image_GetHeight(enemyImg), ENEMY_WIDTH);

    _menuButton = init_button(window_width / 2.f, window_height / 1.25f, 250.f, 75.f,
        BUTTON_TYPE_DEFAULT, "Back to Menu", CP_Color_Create(0, 0, 0, 255), 45.f);

    CP_Image levelBg = NULL;

    switch (level) {
    default:
    case GAME_LEVEL_ONE:
        levelBg = CP_Image_Load("Assets/Levels/level1.png");
        _boss = init_boss_one(BOSS_WIDTH, 30.f, 4500, 60, 1.03f);

        // Init Defense Target Properties
        _tower.maxHealth = _tower.health = 3;
        _tower.height = 300.0;
        _tower.width = 150.0;
        _tower.pos = CP_Vector_Set(level_width / 1.05f, level_height / 2.f);
        _tower.flickerState.isVisible = true;

        // Init Player Properties
        _respawnVec = CP_Vector_Set(_tower.pos.x / 1.15f, _tower.pos.y);
        playerInstance = init_player(_respawnVec, 350.0f, 100, 21, 4.2f);
        playerInstance.playerMaxExp = 20; // Reset exp
        playerInstance.playerLevel = 1; // Reset level

        // Init placeables
        init_barrier_images();
        init_ally_images();

        // Clips
        // Enemy clips
        struct Rectangle oweoMissesEnemyClip = { .pos = CP_Vector_Set(730.f, 290.f), .width = 212.f, .height = 332.f };
        _enemyClips[0] = oweoMissesEnemyClip;

        // Global/World Clips (for all)
        _worldClips[0] = get_tower_hitbox(_tower);
        struct Rectangle topClip = { .pos = CP_Vector_Set(level_width / 2, 50.f), .width = level_height, .height = 110.f };
        struct Rectangle botClip = { .pos = CP_Vector_Set(level_width / 2, 833.f), .width = level_height, .height = 106.f };
        struct Rectangle oweoClip = { .pos = CP_Vector_Set(750.f, 351.f), .width = 140.f, .height = 60.f }; // Oreo
        struct Rectangle missesClip = { .pos = CP_Vector_Set(680.f, 203.5f), .width = 40.f, .height = 15.f }; // Kisses
        struct Rectangle columnClip = { .pos = CP_Vector_Set(1200.f, 600.f), .width = 80.f, .height = 360.f };
        _worldClips[1] = topClip;
        _worldClips[2] = botClip;
        _worldClips[3] = oweoClip;
        _worldClips[4] = missesClip;
        _worldClips[5] = columnClip;
        _worldClipsCount = _obstaclesCount = 6;

        // Boss and grids
        CP_Image bossImg = CP_Image_Load("Assets/boss_lvl1.png");
        float bossHeight = get_preserved_heightf((float)CP_Image_GetWidth(bossImg), (float)CP_Image_GetHeight(bossImg), BOSS_WIDTH);
        
        init_grid(&_enemyGrid, level_width, level_height, ENEMY_WIDTH, enemyHeight);
        init_grid(&_bossGrid, level_width, level_height, BOSS_WIDTH, bossHeight);
        update_grid(&_enemyGrid, _worldClips, _worldClipsCount, _enemyClips, 1);
        update_grid(&_bossGrid, _worldClips, _worldClipsCount, _enemyClips, 1);

        init_grid(&_enemyGridNB, level_width, level_height, ENEMY_WIDTH, enemyHeight);
        init_grid(&_bossGridNB, level_width, level_height, BOSS_WIDTH, bossHeight);
        update_grid(&_enemyGridNB, _worldClips, _worldClipsCount, _enemyClips, 1);
        update_grid(&_bossGridNB, _worldClips, _worldClipsCount, _enemyClips, 1);
        break;
    case GAME_LEVEL_TWO:
        break;
    }

    init_camera(playerInstance, level_width, level_height);
    return levelBg;
}

// Move to next wave or level, if possible
void next_wave_or_level(void) {
    if (_deadEnemies == _totalEnemies && _totalEnemies != 0) {
        if (currentWave < WAVE_3) {
            _hasPlayedNewWaveMusic = false;
            _deadEnemies = 0; // Reset
            currentWave++;
            _waveTimer = 0.f; // Reset wave timer to start next wave
        }
        else {
            // TODO: Do a proper transition
            _isMovingToNextLevel = true;
            switch (_currentLevel) {
            case GAME_LEVEL_ONE:
                CP_Engine_SetNextGameState(level_two_init, level_two_update, level_two_exit);
                break;
            default:
                return;
            }
        }
    }
}

// Returns true if game over, otherwise false
bool try_game_over(float window_width, float window_height, CP_Image cursor) {
    if (_tower.health <= 0) {
        float mouseX = CP_Input_GetMouseX(),
            mouseY = CP_Input_GetMouseY();
        CP_Graphics_ClearBackground(CP_Color_Create(15, 15, 15, 255));
        CP_Settings_TextSize(175.f);
        CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
        CP_Font_DrawText("GAME OVER", window_width / 2.f, window_height / 2.4f);
        render_button(_menuButton);
        update_button_state(&_menuButton, mouseX, mouseY);
        render_cursor(cursor);

        if (!_hasPlayedGameOverMusic) {
            play_sound_advanced(SOUND_MUSIC_GAME_OVER, 1.f, 0);
            _hasPlayedGameOverMusic = true;
        }

        if (_menuButton.state == BUTTON_STATE_CLICKED)
            CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
        return true;
    }
    return false;
}

// Enemy spawning function
void spawn_enemies(float dt) {
    if (_waveTimer >= _waveDelay) {
        if (_spawnInterval > _spawnTimer)
            _spawnTimer += dt;
        else {
            if (!_hasPlayedNewWaveMusic) {
                play_sound_advanced(SOUND_MUSIC_NEW_WAVE_START, 1.f, 0);
                _hasPlayedNewWaveMusic = true;
            }

            float enemySpeed = 0.f;
            int enemyMaxHealth = 0, enemyDmg = 0;
            float nextSpawnDelay = 0.f;
            int nextSpawnDelayIndex = 0;

            switch (_currentLevel) {
#pragma region LEVEL_ONE
            case GAME_LEVEL_ONE:
                switch (currentWave) {
                case WAVE_1:
                    _spawnInterval = 1.65f;
                    enemySpeed = 90.f;
                    enemyMaxHealth = 175;
                    enemyDmg = 20;
                    _enemyMaxAttackCD = 2.f;
                    _roachesMax = 35;
                    nextSpawnDelayIndex = 6;
                    nextSpawnDelay = -0.05f;
                    break;
                case WAVE_2:
                    _spawnInterval = 1.35f;
                    enemySpeed = 105.f;
                    enemyMaxHealth = 250;
                    enemyDmg = 35;
                    _enemyMaxAttackCD = 1.5f;
                    _roachesMax = 55;
                    nextSpawnDelayIndex = 4;
                    nextSpawnDelay = -0.4f;
                    break;
                case WAVE_3:
                    if (_boss.state == BOSS_STATE_UNSPAWNED) {
                        spawn_boss(&_boss, -650.f, 650.f);
                        _totalEnemies++;
                    }
                    _spawnInterval = 1.65f;
                    enemySpeed = 75.f;
                    enemyMaxHealth = 300;
                    enemyDmg = 15;
                    _enemyMaxAttackCD = 1.5f;
                    _roachesMax = 45;
                    nextSpawnDelayIndex = 5;
                    nextSpawnDelay = -0.60f;
                    break;
                }
#pragma endregion
#pragma region LEVEL_TWO
                // Make one
#pragma endregion
            }
            if (_enemySpawnIndex < _roachesMax) {
                _totalEnemies++;
                _roaches[_enemySpawnIndex++] = init_enemy(_spawnMinY, _spawnMaxY, _spawnWidth, enemySpeed, enemyMaxHealth, enemyDmg);
                _spawnTimer = 0.f;
                printf("Enemy spawned. Spawn count: %d | Remaining: %d\n", _enemySpawnIndex, _enemySpawnIndex - _deadEnemies);

                // Increase delay a bit to not overwhelm the player
                if (_enemySpawnIndex % nextSpawnDelayIndex == 0)
                    _spawnTimer = nextSpawnDelay;
            }
        }
    }
    else {
        _enemySpawnIndex = 0;
        _waveTimer += dt;
    }
}

// Placeables function for player
void player_placeables(float mouse_x, float mouse_y, float dt) {
    // Update allies
    update_allies(dt, _roaches, _roachesMax);

    // Barriers
    switch (_currentBarrierState) {
    case BARRIER_IDLE:
        if (_currentAllyState) break;
        // Start preview mode when 'P' is pressed
        if (get_barrier_count() < MAX_BARRIERS && CP_Input_KeyTriggered(KEY_P))
            _currentBarrierState = BARRIER_PREVIEW;
        break;

    case BARRIER_PREVIEW:
        enableShooting = false;
        // Render translucent barrier preview
        CP_Settings_RectMode(CP_POSITION_CENTER);
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 128));
        CP_Image_Draw(get_barrier_image(playerInstance.buffs.barrierLevel), mouse_x, mouse_y, BARRIER_WIDTH, BARRIER_HEIGHT, 128);
        // Place barrier when left mouse button is clicked
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            int barrierIndex = get_barrier_count();
            if (barrierIndex < MAX_BARRIERS) {
                // Initialize barrier properties
                set_barrier_properties(barrierIndex, CP_Vector_Set(mouse_x, mouse_y), BARRIER_WIDTH, BARRIER_HEIGHT, BARRIER_INITIAL_HEALTH);
                increment_barrier_count();
                _currentBarrierState = BARRIER_IDLE;

                // store the barrier in worldClips
                _worldClips[_worldClipsCount++] = barriers[barrierIndex].hitbox;
                update_grid(&_enemyGrid, _worldClips, _worldClipsCount, _enemyClips, 1);
                update_grid(&_bossGrid, _worldClips, _worldClipsCount, _enemyClips, 1);
            }
        }
        break;
    }

    switch (_currentAllyState) {
    case ALLY_IDLE:
        if (_currentBarrierState) break;
        // Start preview mode when 'I' is pressed
        if (CP_Input_KeyTriggered(KEY_I) && get_ally_count() < MAX_ALLIES) {
            _currentAllyState = ALLY_PREVIEW;
        }
        break;
    case ALLY_PREVIEW:
        enableShooting = false;
         // Render translucent preview
        CP_Settings_Fill(CP_Color_Create(0, 255, 0, 128));
        CP_Image_Draw(get_ally_image(playerInstance.buffs.allyLevel), mouse_x, mouse_y, ALLY_WIDTH, ALLY_HEIGHT, 128);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            int allyIndex = get_ally_count();
            if (allyIndex < MAX_ALLIES) {
                init_ally(allyIndex, CP_Vector_Set(mouse_x, mouse_y), ALLY_WIDTH, ALLY_HEIGHT);
                increment_ally_count();
                // Transition back to IDLE
            }
            else
                printf("Max allies reached. Cannot place new ally.\n");
            _currentAllyState = ALLY_IDLE;
        }
        break;
    }
}

// **************************** //
// *        Rendering         * //
// **************************** //
#pragma region Rendering
void render_health_bars(float dt) {
#pragma region Barrier
    for (int i = 0; i < get_barrier_count(); i++) {
        if (barriers[i].isTakingDamage) {
            if (barriers[i].healthBarDisplayTime < BARRIER_HEALTHBAR_DISPLAY_TIME) {
                render_health_bar_rel(barriers[i].hitbox, barriers[i].health, barriers[i].maxHealth);
                barriers[i].healthBarDisplayTime += dt;
            }
            else {
                barriers[i].healthBarDisplayTime = 0;
                barriers[i].isTakingDamage = false;
            }
        }
    }
#pragma endregion

#pragma region Enemies
    for (int i = 0; _roaches[i].currentState; i++) {
        if (_roaches[i].isTakingDamage) {
            if (_roaches[i].healthBarDisplayTime < ENEMY_HEALTHBAR_DISPLAY_TIME) {
                struct Rectangle roachRect = { _roaches[i].pos, _roaches[i].width, _roaches[i].height };
                render_health_bar_rel(roachRect, _roaches[i].health, _roaches[i].maxHealth);
                _roaches[i].healthBarDisplayTime += dt;
            }
            else {
                _roaches[i].healthBarDisplayTime = 0;
                _roaches[i].isTakingDamage = false;
            }
        }
    }
#pragma endregion
}

void render_health_heart(struct Rectangle entity, int health, int maxHealth) {
    // Set position of the first health icon
    float health_icon_w = 45.f;
    float health_icon_h = get_preserved_heightf(CP_Image_GetWidth(_heartImg), CP_Image_GetHeight(_heartImg), health_icon_w);
    float health_icon_y = (entity.pos.y) - (entity.height / 2) - (health_icon_h / 2) - 10.f;
    float health_icon_gap = health_icon_w / 2;
    for (int i = 0; i < health; i++) {
        float health_icon_x = (entity.pos.x - (entity.width / maxHealth)) + (i * (entity.width / maxHealth));
        CP_Image_Draw(_heartImg, health_icon_x, health_icon_y, health_icon_w, health_icon_h, 255);
    }
}

void render_ui(float window_width, float window_height, float mouse_x, float mouse_y) {
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));

    // Wave
    CP_Settings_TextSize(60);
    char currentWaveStr[8];
    sprintf_s(currentWaveStr, sizeof(currentWaveStr), "Wave: %d", currentWave);
    CP_Font_DrawText(currentWaveStr, window_width / 2.f, 50.f);

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

    // Render EXP bar
    enum ProgressBarColor expBarColor = playerInstance.exp >= playerInstance.playerMaxExp ? PROGRESSBAR_COLOR_BLUE : PROGRESSBAR_COLOR_GREEN;
    render_progressbar_custom(125.f, 25.f, 100.f, 100.f, CP_Color_Create(50, 50, 50, 255),
        expBarColor, (float)playerInstance.exp, (float)playerInstance.playerMaxExp);

    // Render player level
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(30.f);
    CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
    char levelStr[10];
    sprintf_s(levelStr, sizeof(levelStr), "Lvl. %d", playerInstance.playerLevel);
    CP_Font_DrawText(levelStr, 25.f, 25.f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
}

void render_new_wave_counter(float window_width, float window_height) {
    if (_waveTimer > (_waveDelay - 1.f)) return;
    char buffer[20];
    sprintf_s(buffer, sizeof(buffer), "Next Wave in %d", (int)(_waveDelay - _waveTimer));
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Settings_TextSize(100.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText(buffer, window_width / 2.f, window_height * 1.f / 5.f);
}

void render_entities(float dt, float mouse_x) {
#pragma region Projectiles
    // Update and render bullets
    for (int i = 0; i < MAX_BULLET_COUNT; i++) {
        update_pos_bullet(&playerInstance.bullets[i], dt);
        render_bullet(playerInstance.bullets[i]);
    }
#pragma endregion

#pragma region Tower
    update_flicker(&_tower.flickerState, dt);
    if (_tower.flickerState.isVisible) {
        render_tower(_tower);
        render_health_heart(get_tower_hitbox(_tower), _tower.health, _tower.maxHealth);
    }
#pragma endregion

#pragma region Enemies
    // Update and render enemies
    sort_enemies(_roaches, _roachesMax); // Sort enemy based on pos asc
    _draw_shadows(playerInstance.pos.x, playerInstance.pos.y, playerInstance.width, playerInstance.height);
    for (int i = 0; _roaches[i].currentState; i++) {
        struct ChocoRoach roach = _roaches[i];
        _draw_shadows(roach.pos.x, roach.pos.y, roach.width, roach.height);
    }
    render_enemies(_roaches, _roachesMax); // Draw enemies instances
#pragma endregion

#pragma region Boss
    // Update and render boss
    if (_boss.state != BOSS_STATE_UNSPAWNED) {
        _draw_shadows(_boss.pos.x, _boss.pos.y, _boss.width, _boss.height);
        render_boss(_boss);
    }
#pragma endregion

#pragma region Render_Player
    // Update and render player
    CP_Vector mouseVec = CP_Vector_Set(mouse_x, 0.f);
    if (playerInstance.pos.x > mouseVec.x)
        playerInstance.width = playerInstance.width > 0 ? -playerInstance.width : playerInstance.width;
    else
        playerInstance.width = playerInstance.width < 0 ? -playerInstance.width : playerInstance.width;
    update_flicker(&playerInstance.flickerState, dt);
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Update hitbox of player
    if (playerInstance.flickerState.isVisible)
        render_player(playerInstance, 255);
#pragma endregion
}
#pragma endregion

// Check for collision
// Currently it is just for player
void check_collision(void) {
    // Prevent player and bullets move into non-movable path
    for (int i = 0; i < _worldClipsCount; i++) {
        struct Rectangle worldClip = _worldClips[i];
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
}

// TODO: Need rework
void enemy_move_attack(float dt) {
#pragma region Enemy Attack State
    for (int i = 0; _roaches[i].currentState; i++) {
        // Attack defense target when collide
        if (are_rects_intersecting(_roaches[i].hitbox, get_tower_hitbox(_tower))) {
            _roaches[i].currentState = ENEMY_STATE_ATTACKING;
            _enemyAttackTarget = TARGET_TOWER;
            continue;
        }
        // Attack player when collide
        if (are_rects_intersecting(_roaches[i].hitbox, playerInstance.hitbox)) { // When collide attack player
            _roaches[i].currentState = ENEMY_STATE_ATTACKING;
            _enemyAttackTarget = TARGET_PLAYER;
            continue;
        }

        // Attack barrier when collide
        for (int j = 0; j < get_barrier_count(); j++) {
            if (are_rects_intersecting(_roaches[i].hitbox, barriers[j].hitbox)) {
                _roaches[i].currentState = ENEMY_STATE_ATTACKING;
                _enemyAttackTarget = TARGET_BARRIER;
                _roaches[i].barrierAttack = j;
                break;
            }
        }
        if (_roaches[i].barrierAttack >= 0)
            continue;

        struct Circle radarRoach = { .pos = _roaches[i].pos, .diameter = 5 * fabs(_roaches[i].width) }; // define enemy radar
        if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) { // Check enemy near with player
            _roaches[i].currentState = ENEMY_STATE_MOVING_TO_PLAYER;
            continue;
        }
        _roaches[i].currentState = ENEMY_STATE_MOVING_TO_TOWER;
    }
#pragma endregion

    int barriersDestroyed[MAX_BARRIERS];
    int barriersDestroyedCount = 0;
    bool isDestroyed = false;

    for (int i = 0; i < _roachesMax; i++) {
#pragma region Enemy Movement & Attack 
        if (_roaches[i].currentState) { // if enemy is alive
            int barriersDestroyed[MAX_BARRIERS] = { 0 };
            int barriersDestroyedCount = 0;
            bool isDestroyed = false;
            _roaches[i].attackCooldown = max(0, _roaches[i].attackCooldown - dt);
            switch (_roaches[i].currentState) {
            case ENEMY_STATE_MOVING_TO_TOWER: {
                enemy_move_to_target(&_roaches[i], get_tower_hitbox(_tower), dt, _enemyGrid.nodes, _enemyGridNB.nodes, _enemyGrid.nodeCount);
                break;
            }

            case ENEMY_STATE_MOVING_TO_PLAYER: {
                enemy_move_to_target(&_roaches[i], playerInstance.hitbox, dt, _enemyGrid.nodes, _enemyGridNB.nodes, _enemyGrid.nodeCount);
                break;
            }

            case ENEMY_STATE_ATTACKING: {
                // stop movement
                _roaches[i].moveDirection = CP_Vector_Zero();

                // attack animation here...


                // if animation end then...
                switch (_enemyAttackTarget) {
                case TARGET_PLAYER:
                    if (!player_take_damage(&playerInstance, _roaches[i].dmg)) { // Returns 0 if player dies from damage
                        if (tower_revive_player(&_tower, &playerInstance)) // Returns 1 if tower is still alive after reviving
                            playerInstance.pos = _respawnVec;
                    }
                    break;
                case TARGET_TOWER:
                    tower_take_damage(&_tower, 1);
                    break;
                case TARGET_BARRIER: {
                    for (int j = 0; j < barriersDestroyedCount; j++) {
                        if (barriersDestroyed[j] == _roaches[i].barrierAttack)
                            if (barriersDestroyed[j] == _roaches[i].barrierAttack) {
                                isDestroyed = true;
                                _roaches[i].barrierAttack = -1;
                            }
                    }
                    if (_roaches[i].attackCooldown <= 0 && !isDestroyed) {
                        _roaches[i].attackCooldown = _enemyMaxAttackCD;
                        if (!barrier_take_damage(_roaches[i].barrierAttack, _roaches[i].dmg)) {
                            barriersDestroyed[barriersDestroyedCount++] = _roaches[i].barrierAttack;
                            int worldClipsIndex = _obstaclesCount + _roaches[i].barrierAttack;
                            if (worldClipsIndex != _worldClipsCount - 1) {
                                struct Rectangle tmp = _worldClips[_worldClipsCount - 1];
                                _worldClips[_worldClipsCount - 1] = _worldClips[worldClipsIndex];
                                _worldClips[worldClipsIndex] = tmp;
                            }
                            _worldClipsCount--;
                            update_grid(&_enemyGrid, _worldClips, _worldClipsCount, _enemyClips, 1);
                            update_grid(&_bossGrid, _worldClips, _worldClipsCount, _enemyClips, 1);
                            _roaches[i].barrierAttack = -1; // make index of barrier to attack to negative
                        }
                    }
                    break;
                }
                }
                break;
            }

            case ENEMY_STATE_IDLE:
                _roaches[i].moveDirection = CP_Vector_Zero();
                break;
            }
            sort_barriers();
            CP_Vector hitboxPos = { _roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(_roaches[i].width), _roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(_roaches[i].height) };
            update_rect_hitbox(&_roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Update hitbox of enemy
#pragma endregion

#pragma region Enemy Taking Damage
            for (int j = 0; j < MAX_BULLET_COUNT; j++) {
                if (playerInstance.bullets[j].isShooting) {
                    struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
                    if (are_circle_rect_intersecting(bullet, _roaches[i].hitbox)) {
                        if (enemy_take_damage(&_roaches[i], playerInstance.bullets[j].dmg)) {
                            _deadEnemies++;
                            // Base exp on this formula:
                            // (max health / speed) * (max enemies / 10)
                            int expGained = _get_exp(_roaches[i].maxHealth, _roaches[i].speed, _roachesMax);
                            enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained);

                            // Apply the buff if one was granted
                            if (randomBuff != 0) {
                                apply_buff(&playerInstance, randomBuff, barriers, allies);
                            }
                        }
                        // Cancel bullet after hit enemy
                        playerInstance.bullets[j].isShooting = FALSE;
                    }
                }
            }

            for (int a = 0; a < get_ally_count(); a++) {  // Loop through all allies
                struct Ally ally = allies[a];    // Get a reference to the current ally
                for (int j = 0; j < MAX_BULLET_COUNT; j++) {  // Loop through each ally's bullets
                    if (ally.bullets[j].isShooting) {
                        struct Circle bullet = { .pos = ally.bullets[j].pos, .diameter = ally.bullets[j].diameter };
                        if (are_circle_rect_intersecting(bullet, _roaches[i].hitbox)) {
                            if (enemy_take_damage(&_roaches[i], ally.bullets[j].dmg)) {
                                _deadEnemies++;
                                int expGained = _get_exp(_roaches[i].maxHealth, _roaches[i].speed, _roachesMax);
                                enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained);

                                // Apply the buff if one was granted
                                if (randomBuff != 0) {
                                    apply_buff(&playerInstance, randomBuff, barriers, allies);
                                }
                            }
                            ally.bullets[j].isShooting = FALSE;
                        }
                    }
                }
            }
#pragma endregion
        }
        // can use else statement here for dying animation...
    }

#pragma region Boss Movement & Attack
    switch (_boss.state) {
    case BOSS_STATE_MOVING_TO_TOWER:
        _boss.move(&_boss, playerInstance, get_tower_hitbox(_tower), dt, _bossGrid.nodes, _bossGrid.nodeCount);
        break;

    case BOSS_STATE_MOVING_TO_PLAYER:
        _boss.move(&_boss, playerInstance, playerInstance.hitbox, dt, _bossGrid.nodes, _bossGrid.nodeCount);
        break;

    case BOSS_STATE_IDLE:
        _boss.moveDirection = CP_Vector_Zero();
        break;
    }

    if (_boss.canPrimaryAttack(_boss, playerInstance))
        _boss.primaryAttack(&_boss, &playerInstance, dt);
    else if (_boss.canSecondaryAttack(_boss, playerInstance))
        _boss.secondaryAttack(&_boss, &playerInstance, dt);
#pragma endregion

#pragma region Boss Taking Damage
    for (int j = 0; j < MAX_BULLET_COUNT; j++) {
        if (playerInstance.bullets[j].isShooting) {
            struct Circle bullet = { .pos = playerInstance.bullets[j].pos, .diameter = playerInstance.bullets[j].diameter };
            if (are_circle_rect_intersecting(bullet, _boss.hitbox)) {
                if (boss_take_damage(&_boss, playerInstance.bullets[j].dmg)) {
                    _deadEnemies++;
                    float expGained = _get_exp(_boss.maxHealth, _boss.speed, 1);
                    enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained * 12.f);

                    // Apply the buff if one was granted
                    if (randomBuff != 0) {
                        apply_buff(&playerInstance, randomBuff, barriers, allies);
                    }
                    dispose_boss(&_boss);
                }
                playerInstance.bullets[j].isShooting = FALSE;
            }
        }
    }

    for (int a = 0; a < get_ally_count(); a++) {  // Loop through all allies
        struct Ally ally = allies[a];    // Get a reference to the current ally
        for (int j = 0; j < MAX_BULLET_COUNT; j++) {  // Loop through each ally's bullets
            if (ally.bullets[j].isShooting) {
                struct Circle bullet = { .pos = ally.bullets[j].pos, .diameter = ally.bullets[j].diameter };
                if (are_circle_rect_intersecting(bullet, _boss.hitbox)) {
                    if (boss_take_damage(&_boss, ally.bullets[j].dmg)) {
                        _deadEnemies++;
                        float expGained = _get_exp(_boss.maxHealth, _boss.speed, 1);
                        enum BuffType randomBuff = update_buff_meter(&playerInstance, expGained * 12.f);

                        // Apply the buff if one was granted
                        if (randomBuff != 0) {
                            apply_buff(&playerInstance, randomBuff, barriers, allies);
                        }
                        dispose_boss(&_boss);
                    }
                    ally.bullets[j].isShooting = FALSE;
                }
            }
        }
    }
#pragma endregion
}

#if _DEBUG
void debug_render_grid(void) {
    // draw_grid(enemyGrid.nodes, enemyGrid.nodeCount, cam); // draw enemy grid for debugging
    // draw_grid(bossGrid.nodes, bossGrid.nodeCount, cam); // draw boss grid for debugging
    // draw_grid(enemyGridNB.nodes, enemyGrid.nodeCount, cam); // draw enemy grid for debugging
    // draw_grid(bossGridNB.nodes, bossGrid.nodeCount, cam); // draw boss grid for debugging
}
#endif

void dispose_resources(void) {
    if (!_isMovingToNextLevel)
        CP_Image_Free(&playerInstance.img);
    CP_Image_Free(&_heartImg);
    
    for (int i = 0; i < _roachesMax; i++) {
        struct ChocoRoach roach = _roaches[i];
        if (roach.img != NULL)
            CP_Image_Free(&roach.img);
    }
    memset(_roaches, 0, sizeof(_roaches));
    if (_boss.img != NULL)
        CP_Image_Free(&_boss.img);
    free(_enemyGrid.nodes);
    free(_enemyGridNB.nodes);
    free(_bossGrid.nodes);
    free(_bossGridNB.nodes);
    dispose_barriers();
    dispose_allies();
}
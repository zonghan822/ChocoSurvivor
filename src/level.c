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
#include "projectile.h"
#include "buff.h"
#include "optionsmenu.h"

// Macros
#define MAX_ENEMY_COUNT 100

#pragma region Spawn Properties
static float _spawnMinY = 0.f, _spawnMaxY = 0.f, _spawnWidth = 0.f;
static float _spawnInterval = 0.0f, _spawnTimer = 0.f;      // Spawn interval
static float _waveDelay = 0.f, _waveTimer = 0.f;            // Wave delay
static int _enemySpawnIndex = 0;
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
static float _currentBossHealthWidth;
static float _enemyMaxAttackCD = 0.f;
static float enemySpeed = 0.f;
static float bossSpeed = 0.f;
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
static struct Rectangle _worldClips[35];
static struct Rectangle _playerClips[1];
static struct Rectangle _enemyClips[4];
static int _obstaclesCount = 0;
static int _worldClipsCount = 0;
static Grid _enemyGrid, _bossGrid, _enemyGridNB, _bossGridNB;
#pragma endregion

#pragma region Ally Properties
static enum AllyState _currentAllyState = ALLY_IDLE;
#pragma endregion

#pragma region Buff Properties
BuffSlot buffSlot;
BuffType tempSlot;
#pragma endregion


enum SpawnWave currentWave;

static struct Button _gameOverMenuButton;
static struct Button _victoryMenuButton;
static bool _isMovingToNextLevel;
static bool _hasPlayedGameOverMusic,
    _hasPlayedNewWaveMusic;
static enum GameLevel _currentLevel;
static float _levelWidth, _levelHeight;
static CP_Image _heartImg;
static CP_Image _playerPortraitImg;

bool isPaused, isWin;
static struct Button _resumeButton, _pauseBackToMenuButton, _optionsButton;

// ***************** Naming Convention ***************** //
//  No prefix           if level-related                 //
//  player_xyz          if player-related                //
//  render_xyz          if render-related                //
//  try_xyz             if returns a bool                //
//  etc                                                  //
// ***************************************************** //

#pragma region Private Methods
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
#pragma endregion

// Returns an instance of the level background
CP_Image init_level(enum GameLevel level, float level_width, float level_height, float window_width, float window_height) {
    _currentLevel = level;
    currentWave = WAVE_1;
    _levelWidth = level_width;
    _levelHeight = level_height;
    _heartImg = CP_Image_Load("Assets/heart.png");
    _playerPortraitImg = CP_Image_Load("Assets/player_portrait.png");
    _currentBossHealthWidth = 0.f;

    // Reset values
    isPaused = isWin = false;
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
    playerProjectiles.activeCount = 0;
    memset(&playerProjectiles, 0, sizeof(playerProjectiles.projectiles));
    memset(&allyProjectiles, 0, sizeof(allyProjectiles->projectiles));
    memset(&buffSlot, 0, sizeof(buffSlot));

    float halfWindowWidth = window_width / 2.f,
        halfWindowHeight = window_height / 2.f;
    _resumeButton = init_button(halfWindowWidth, halfWindowHeight - 75.f, 200.f, 50.f,
        BUTTON_TYPE_SUCCESS, "Resume", CP_Color_Create(0, 0, 0, 255), 24.0f);
    _optionsButton = init_button(halfWindowWidth, halfWindowHeight, 200.f, 50.f,
        BUTTON_TYPE_DEFAULT, "Options", CP_Color_Create(255, 255, 255, 255), 24.0f);
    _pauseBackToMenuButton = init_button(halfWindowWidth, halfWindowHeight + 75.f, 200.f, 50.f,
        BUTTON_TYPE_WARNING, "Back to Menu", CP_Color_Create(255, 255, 255, 255), 24.0f);

    // Can move to level specific cases
    _spawnMinY = level_height * 1 / 5;
    _spawnMaxY = level_height * 4 / 5;
    _spawnWidth = 300.f;

    CP_Image enemyImg = CP_Image_Load("Assets/choco_roach.png");
    float enemyHeight = get_preserved_heightf((float)CP_Image_GetWidth(enemyImg),
        (float)CP_Image_GetHeight(enemyImg), ENEMY_WIDTH);

    _gameOverMenuButton = init_button(halfWindowWidth, window_height / 1.25f, 300.f, 75.f,
        BUTTON_TYPE_DEFAULT, "Back to Menu", CP_Color_Create(255, 255, 255, 255), 40.f);
    _victoryMenuButton = init_button(halfWindowWidth, halfWindowHeight + 75.f, 200.f, 50.f,
        BUTTON_TYPE_DEFAULT, "Back to Menu", CP_Color_Create(255, 255, 255, 255), 24.0f);

    CP_Image levelBg = NULL;

    switch (level) {
    default:
    case GAME_LEVEL_ONE:
        levelBg = CP_Image_Load("Assets/Levels/level1.png");
        bossSpeed = 20.f;
        _boss = init_boss_one(BOSS_WIDTH, bossSpeed, 4900, 60, 1.03f);

        CP_Image towerImg = CP_Image_Load("Assets/tower.png");

        // Init Defense Target Properties
        _tower.maxHealth = _tower.health = 3;
        _tower.width = 170.0;
        _tower.height = get_preserved_heightf(CP_Image_GetWidth(towerImg), CP_Image_GetHeight(towerImg), _tower.width);
        _tower.pos = CP_Vector_Set(level_width / 1.04f, level_height / 2.f);
        _tower.flickerState.isVisible = true;
        _tower.img = towerImg;
        // Init Player Properties
        _respawnVec = CP_Vector_Set(_tower.pos.x / 1.1f, _tower.pos.y + (_tower.height / 5.5f));
        playerInstance = init_player(_respawnVec, 350.0f, 105, 20, 4.2f);
        playerInstance.playerMaxExp = 20; // Reset exp
        playerInstance.playerLevel = 1; // Reset level

        // Init Buffs
        memset(&activeBuffs, 0, sizeof(activeBuffs));
        tempSlot = 0;

        // Init placeables
        init_barrier_images();
        init_ally_images();

        // Clips
        // Enemy clips
        struct Rectangle oweoMissesEnemyClip = { .pos = CP_Vector_Set(885.f, 290.f), .width = 212.f, .height = 332.f };
        struct Rectangle enemyClip1 = { .pos = CP_Vector_Set(650.f, 450.f), .width = 70.f, .height = 150.f };
        struct Rectangle enemyClip2 = { .pos = CP_Vector_Set(1130.f, 450.f), .width = 70.f, .height = 150.f };
        struct Rectangle enemyClip3 = { .pos = CP_Vector_Set(1510.f, 390.f), .width = 450.f, .height = 70.f };
        _enemyClips[0] = oweoMissesEnemyClip;
        _enemyClips[1] = enemyClip1;
        _enemyClips[2] = enemyClip2;
        _enemyClips[3] = enemyClip3;

        // Global/World Clips (for all)
        _worldClips[0] = get_tower_hitbox(_tower);
        struct Rectangle topClip = { .pos = CP_Vector_Set(level_width / 2, 50.f), .width = level_width , .height = 110.f };
        struct Rectangle botClip = { .pos = CP_Vector_Set(level_width / 2, 833.f), .width = level_width, .height = 106.f };
        struct Rectangle oweoClip = { .pos = CP_Vector_Set(915.f, 351.f), .width = 120.f, .height = 60.f }; // Oreo
        struct Rectangle missesClip = { .pos = CP_Vector_Set(830.f, 190.f), .width = 40.f, .height = 5.f }; // Kisses
        struct Rectangle columnClip = { .pos = CP_Vector_Set(1520.f, 600.f), .width = 80.f, .height = 360.f };
        _worldClips[1] = topClip;
        _worldClips[2] = botClip;
        _worldClips[3] = oweoClip;
        _worldClips[4] = missesClip;
        _worldClips[5] = columnClip;
        _worldClipsCount = _obstaclesCount = 6;

        // Boss and grids
        CP_Image bossImg = CP_Image_Load("Assets/boss_lvl1.png");
        float bossHeight = get_preserved_heightf((float)CP_Image_GetWidth(bossImg), (float)CP_Image_GetHeight(bossImg), BOSS_WIDTH);
        
        init_grid(&_enemyGrid, level_width, level_height, 0.8 * ENEMY_WIDTH, 0.75 * enemyHeight);
        init_grid(&_bossGrid, level_width, level_height, 77.f / 144.f * BOSS_WIDTH, 166.f / 251.f * bossHeight);
        update_grid(&_enemyGrid, _worldClips, _worldClipsCount, _enemyClips, 4);
        update_grid(&_bossGrid, _worldClips, _worldClipsCount, _enemyClips, 4);

        init_grid(&_enemyGridNB, level_width, level_height, 0.8 * ENEMY_WIDTH, 0.75 * enemyHeight);
        init_grid(&_bossGridNB, level_width, level_height, 77.f / 144.f * BOSS_WIDTH, 166.f / 251.f * bossHeight);
        update_grid(&_enemyGridNB, _worldClips, _worldClipsCount, _enemyClips, 4);
        update_grid(&_bossGridNB, _worldClips, _worldClipsCount, _enemyClips, 4);
        break;
    case GAME_LEVEL_TWO:
        break;
    }

    init_camera(playerInstance, level_width, level_height);
    return levelBg;
}

bool try_pause(float window_width, float window_height) {
    if (CP_Input_KeyTriggered(KEY_ESCAPE))
        isPaused = !isPaused;
    if (!isPaused) return false;
    if (isPaused) {
        CP_Settings_StrokeWeight(0.f);
        float mouseX = CP_Input_GetMouseX(),
            mouseY = CP_Input_GetMouseY();

        // Render a dim background overlay
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 210));
        CP_Graphics_DrawRect(window_width / 2.f, window_height / 2.f, window_width, window_height);

        CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
        CP_Settings_TextSize(85.f);
        CP_Font_DrawText("PAUSED", window_width / 2.f, window_height / 4.f);

        // Render buttons
        render_button(_resumeButton);
        render_button(_pauseBackToMenuButton);
        render_button(_optionsButton);

        update_button_state(&_resumeButton, mouseX, mouseY);
        update_button_state(&_pauseBackToMenuButton, mouseX, mouseY);
        update_button_state(&_optionsButton, mouseX, mouseY);

        // Check for button clicks and handle actions
        if (_resumeButton.state == BUTTON_STATE_CLICKED) {
            isPaused = false;
            return false;
        }
        if (_pauseBackToMenuButton.state == BUTTON_STATE_CLICKED) {
            CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
            return false;
        }
        if (_optionsButton.state == BUTTON_STATE_CLICKED || is_options_menu_open())
            render_options_menu(mouseX, mouseY, window_width, window_height);
        CP_Settings_StrokeWeight(3.f);
        return true;
    }
}

// Move to next wave or level, if possible
void next_wave_or_level(void) {
    if (_deadEnemies <= 0) return;

    if (_deadEnemies == (_boss.state == BOSS_STATE_UNSPAWNED ? _roachesMax : _roachesMax + 1)) {
        if (currentWave < WAVE_3) {
            _hasPlayedNewWaveMusic = false;
            _deadEnemies = 0; // Reset
            _roachesMax = 0;
            currentWave++;
            _waveTimer = 0.f; // Reset wave timer to start next wave
        }
        else {
            isWin = true;
            // TODO: Do a proper transition
            _isMovingToNextLevel = true;
            //switch (_currentLevel) {
            //case GAME_LEVEL_ONE:
            //    // CP_Engine_SetNextGameState(level_two_init, level_two_update, level_two_exit);
            //    break;
            //default:
            //    return;
            //}
        }
    }
}

void _render_victory_page(float window_width, float window_height) {
    if (!isWin) return;
    // TODO: Victory screen
    CP_Settings_StrokeWeight(0.f);
    float mouseX = CP_Input_GetMouseX(),
        mouseY = CP_Input_GetMouseY();

    // Render a dim background overlay
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 210));
    CP_Graphics_DrawRect(window_width / 2.f, window_height / 2.f, window_width, window_height);

    CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
    CP_Settings_TextSize(85.f);
    CP_Font_DrawText("YOU SAVED THE CHOCOLATE!!!", window_width / 2.f, window_height / 4.f);

    // Render buttons
    render_button(_victoryMenuButton);
    update_button_state(&_victoryMenuButton, mouseX, mouseY);

    if (_victoryMenuButton.state == BUTTON_STATE_CLICKED) {
        CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
    }
    CP_Settings_StrokeWeight(3.f);
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
        render_button(_gameOverMenuButton);
        update_button_state(&_gameOverMenuButton, mouseX, mouseY);
        render_cursor(cursor);

        if (!_hasPlayedGameOverMusic) {
            play_sound_advanced(SOUND_MUSIC_GAME_OVER, 1.f, 0);
            _hasPlayedGameOverMusic = true;
        }

        if (_gameOverMenuButton.state == BUTTON_STATE_CLICKED)
            CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
        return true;
    }
    return false;
}

// Enemy spawning function
void spawn_enemies(float dt) {
    if (isPaused || isWin) return;

    if (_waveTimer >= _waveDelay) {
        if (_spawnInterval > _spawnTimer)
            _spawnTimer += dt;
        else {
            if (!_hasPlayedNewWaveMusic) {
                play_sound_advanced(SOUND_MUSIC_NEW_WAVE_START, 1.f, 0);
                _hasPlayedNewWaveMusic = true;
            }

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
                    _spawnInterval = 1.3f;
                    enemySpeed = 110.f;
                    enemyMaxHealth = 215;
                    enemyDmg = 35;
                    _enemyMaxAttackCD = 1.5f;
                    _roachesMax = 50;
                    nextSpawnDelayIndex = 6;
                    nextSpawnDelay = -1.5f;
                    break;
                case WAVE_3:
                    if (_boss.state == BOSS_STATE_UNSPAWNED)
                        spawn_boss(&_boss, -650.f, 650.f);
                    _spawnInterval = 1.65f;
                    enemySpeed = 75.f;
                    enemyMaxHealth = 300;
                    enemyDmg = 15;
                    _enemyMaxAttackCD = 1.5f;
                    _roachesMax = 45;
                    nextSpawnDelayIndex = 5;
                    nextSpawnDelay = -0.75f;
                    break;
                }
            #pragma endregion

            #pragma region LEVEL_TWO
            // Make one
            #pragma endregion
            }
            if (_enemySpawnIndex < _roachesMax) {
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
    if (isPaused || isWin) return;

    // Update allies
    ally_shoot(dt, _roaches, _roachesMax);
    sort_barriers();
    for (int i = 0; i < get_barrier_count(); i++) {
        _worldClips[_obstaclesCount + i] = barriers[i].hitbox;
    }
    _worldClipsCount = _obstaclesCount + get_barrier_count();
    update_grid(&_enemyGrid, _worldClips, _worldClipsCount, _enemyClips, 4);
    update_grid(&_bossGrid, _worldClips, _worldClipsCount, _enemyClips, 4);

    // Right click to cancel any previews
    if (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT)) {
        _currentBarrierState = 0;
        _currentAllyState = 0;
    }

    // Barriers
    switch (_currentBarrierState) {
    case BARRIER_IDLE:
        if (_currentAllyState) break;
        // Start preview mode when 'B' is pressed
        if (get_barrier_count() < MAX_BARRIERS && CP_Input_KeyTriggered(KEY_B))
            _currentBarrierState = BARRIER_PREVIEW;
        break;

    case BARRIER_PREVIEW: {
        enableShooting = false;
        // Render translucent barrier preview
        CP_Settings_RectMode(CP_POSITION_CENTER);
        CP_Settings_Fill(CP_Color_Create(0, 0, 255, 128));
        int barrierLvl = 0;
        for (int i = 0; i < activeBuffs.buffCount; i++) {
            if (activeBuffs.buffs[i].type == BARRIER_LVL_UP) {
                barrierLvl = activeBuffs.buffs[i].level;
                break;
            }
        }
        CP_Image_Draw(get_barrier_image(barrierLvl), mouse_x, mouse_y, BARRIER_WIDTH, BARRIER_HEIGHT, 128);
        // Place barrier when left mouse button is clicked
        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            int barrierIndex = get_barrier_count();
            if (barrierIndex < MAX_BARRIERS)
                place_barrier(barrierIndex, mouse_x, mouse_y);
            _currentBarrierState = BARRIER_IDLE;
        }
        break;
    }
    }

    switch (_currentAllyState) {
    case ALLY_IDLE:
        if (_currentBarrierState) break;
        // Start preview mode when 'V' is pressed
        if (CP_Input_KeyTriggered(KEY_V) && get_ally_count() < MAX_ALLIES) {
            _currentAllyState = ALLY_PREVIEW;
        }
        break;
    case ALLY_PREVIEW:
        enableShooting = false;
         // Render translucent preview
        CP_Settings_Fill(CP_Color_Create(0, 255, 0, 128));
        int allyLvl = 0;
        for (int i = 0; i < activeBuffs.buffCount; i++) {
            if (activeBuffs.buffs[i].type == ALLY_LVL_UP) {
                allyLvl = activeBuffs.buffs[i].level;
                break;
            }
        }
        CP_Image prevImg = get_ally_image(allyLvl);
        float preservedHeight = get_preserved_heightf(CP_Image_GetWidth(prevImg), CP_Image_GetHeight(prevImg), ALLY_WIDTH);
        CP_Image_Draw(prevImg, mouse_x, mouse_y, ALLY_WIDTH, preservedHeight, 128);

        if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
            int allyIndex = get_ally_count();
            if (allyIndex < MAX_ALLIES)
                place_ally(allyIndex, mouse_x, mouse_y);
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
                render_health_bar(barriers[i].hitbox, barriers[i].health, barriers[i].maxHealth);
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
                render_health_bar(roachRect, _roaches[i].health, _roaches[i].maxHealth);
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

void render_ui(float window_width, float window_height, float mouse_x, float mouse_y, float dt) {
    // Info panel
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 150));
    CP_Graphics_DrawRect(window_width / 2.f, 45.f, window_width + 10.f, 110.f);

    #pragma region Wave Indicator
    // Wave
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255)); // White
    CP_Settings_TextSize(60);
    char currentWaveStr[10];
    if (_boss.state != BOSS_STATE_UNSPAWNED) {
        CP_Settings_Fill(CP_Color_Create(255, 127, 127, 255));
        sprintf_s(currentWaveStr, sizeof(currentWaveStr), "BOSS WAVE");
    }
    else if (_waveTimer >= (_waveDelay - 1.f)) {
        CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
        sprintf_s(currentWaveStr, sizeof(currentWaveStr), "Wave: %d", currentWave);
    }
    CP_Font_DrawText(currentWaveStr, window_width / 2.f, 35.f);
    #pragma endregion

    // Render player portrait (image source is a square)
    CP_Image_Draw(_playerPortraitImg, 40.f, 45.f, 60.f, 60.f, 200);

    // Render Health bar
    render_progressbar_custom(150.f, 23.f, 270.f, 30.f, CP_Color_Create(50, 50, 50, 255),
        PROGRESSBAR_COLOR_RED, (float)playerInstance.health, (float)playerInstance.maxHealth);

    // Render EXP bar
    enum ProgressBarColor expBarColor = playerInstance.exp >= playerInstance.playerMaxExp ? PROGRESSBAR_COLOR_BLUE : PROGRESSBAR_COLOR_GREEN;
    render_progressbar_custom(150.f, 23.f, 270.f, 70.f, CP_Color_Create(50, 50, 50, 255),
        expBarColor, (float)playerInstance.exp, (float)playerInstance.playerMaxExp);

    // Render player level and health
    CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(30.f);
    char levelStr[12];
    char healthStr[12];
    sprintf_s(levelStr, sizeof(levelStr), "Lvl. %d", playerInstance.playerLevel);
    sprintf_s(healthStr, sizeof(healthStr), "%d / %d", playerInstance.health, playerInstance.maxHealth);
    CP_Font_DrawText(levelStr, 180.f, 70.f);
    CP_Font_DrawText(healthStr, 180.f, 30.f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    // Render remaining enemies
    if (_waveTimer >= (_waveDelay - 1.f) && _roachesMax != 0) {
        CP_Settings_Fill(CP_Color_Create(200, 200, 200, 255));
        CP_Settings_TextSize(25.f);
        char remainingStr[16];
        sprintf_s(remainingStr, sizeof(remainingStr), "Remaining: %d", (_roachesMax - _deadEnemies) + (_boss.state != BOSS_STATE_UNSPAWNED));
        CP_Font_DrawText(remainingStr, window_width / 2.f, 75.f);
    }

    // Render placeables on UI
    CP_Settings_Fill(CP_Color_Create(220, 220, 220, 255));
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Settings_TextSize(30.f);
    char barrierAmountStr[7];
    char allyAmountStr[7];
    sprintf_s(barrierAmountStr, sizeof(barrierAmountStr), "%d / %d", get_barrier_count(), MAX_BARRIERS);
    sprintf_s(allyAmountStr, sizeof(allyAmountStr), "%d / %d", get_ally_count(), MAX_ALLIES);
    CP_Font_DrawText(barrierAmountStr, window_width - 120.f, 30.f);
    CP_Font_DrawText(allyAmountStr, window_width - 120.f, 70.f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText("Barricades:", window_width - 140.f, 30.f);
    CP_Font_DrawText("Sentries:", window_width - 140.f, 70.f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

    // Render boss health bar
    if (_boss.state != BOSS_STATE_UNSPAWNED && _boss.state != BOSS_STATE_DIED) {
        CP_Settings_StrokeWeight(5.f);
        const float bossHpMaxWidth = 1150.f,
            bossHpMaxHeight = 30.f;
        float currentBossHpWidth,
            currentBossHpX = window_width / 2.f;

        // Increase the health bar like an animation
        if (_currentBossHealthWidth < bossHpMaxWidth)
            _currentBossHealthWidth += 250.f * dt;
        else
            _currentBossHealthWidth = bossHpMaxWidth; // Correct the width

        // Set the value of health bar according to conditions
        if (_boss.health == _boss.maxHealth)
            currentBossHpWidth = _currentBossHealthWidth;
        else { // This is for correcting the health bar, and perform calculations if boss is damaged
            _currentBossHealthWidth = bossHpMaxWidth;
            currentBossHpWidth = ((float)_boss.health / (float)_boss.maxHealth) * bossHpMaxWidth;
            currentBossHpX -= (bossHpMaxWidth - currentBossHpWidth) / 2.f;
        }


        // Draw health bar rect
        // Need to use POSITION_CENTER for animation

        // Base
        CP_Settings_Fill(CP_Color_Create(100, 100, 100, 255));
        CP_Graphics_DrawRect(window_width / 2.f, window_height - 60.f, _currentBossHealthWidth, bossHpMaxHeight);

        // Value
        CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
        CP_Graphics_DrawRect(currentBossHpX, window_height - 60.f, currentBossHpWidth, bossHpMaxHeight);
        CP_Settings_StrokeWeight(3.f);
    }


    #pragma region Buffs
    render_buff_slot(&buffSlot, window_width, window_height);
    render_buff(&activeBuffs, window_width, window_height, isPaused || isWin ? 0.f : dt);
    #pragma endregion
    
}

void render_new_wave_counter(float window_width, float window_height) {
    if (_waveTimer > (_waveDelay - 1.f)) return;
    char buffer[20];
    sprintf_s(buffer, sizeof(buffer), "Wave Starting in %d", (int)(_waveDelay - _waveTimer));
    CP_Settings_Fill(CP_Color_Create(255, 0, 0, 255));
    CP_Settings_TextSize(100.0f);
    CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
    CP_Font_DrawText(buffer, window_width / 2.f, window_height * 1.f / 5.f);
}

void render_entities(float dt, float mouse_x) {
#pragma region Projectiles
    // Update and render bullets
    if (!isPaused && !isWin)
        update_all_projectiles(dt);
    render_all_projectiles();
#pragma endregion

#pragma region Tower
    if (!isPaused && !isWin)
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
        render_boss_one_sec_atk_telegraph(_boss, playerInstance);
        render_boss_one_pri_atk_telegraph();
        render_boss(_boss);
    }
#pragma endregion

#pragma region Render_Player
    // Update and render player
    if (!isPaused && !isWin) {
        CP_Vector mouseVec = CP_Vector_Set(mouse_x, 0.f);
        if (playerInstance.pos.x > mouseVec.x)
            playerInstance.width = playerInstance.width > 0 ? -playerInstance.width : playerInstance.width;
        else
            playerInstance.width = playerInstance.width < 0 ? -playerInstance.width : playerInstance.width;
        update_flicker(&playerInstance.flickerState, dt);
    }
    update_rect_hitbox(&playerInstance.hitbox, 0.0f, 0.0f, playerInstance.pos); // Update hitbox of player
    if (playerInstance.flickerState.isVisible)
        render_player(playerInstance, 255);
#pragma endregion
}
#pragma endregion

// Check for collision
// Currently it is just for player
void check_collision(void) {
    if (isPaused || isWin) return;

    if (!playerInstance.health)
        if (tower_revive_player(&_tower, &playerInstance))
            playerInstance.pos = _respawnVec;

    // Player
    // Prevent player from moving into non-movable path
    for (int i = 0; i < _worldClipsCount; i++) {
        struct Rectangle worldClip = _worldClips[i];
        if (are_rects_intersecting(playerInstance.hitbox, worldClip))
            clamp_pos_clip_rel(&playerInstance.pos, playerInstance.hitbox, worldClip);
    }

    // Player Projectiles
    for (int j = 0; j < playerProjectiles.activeCount; j++) {
        if (playerProjectiles.projectiles[j].isActive) {
            struct Projectile projectile = playerProjectiles.projectiles[j];
            struct Circle projectileHitbox = { .pos = projectile.pos, .diameter = projectile.diameter };

            // Obstacles
            for (int i = 0; i < _worldClipsCount; i++) {
                struct Rectangle worldClip = _worldClips[i];
                if (are_circle_rect_intersecting(projectileHitbox, worldClip)) {// bullet cannot go through walls and barricades...
                    remove_projectile(&playerProjectiles, j);
                    break;
                }
            }

            // Common Enemies (roaches)
            for (int i = 0; i < _roachesMax; i++) {
                if (are_circle_rect_intersecting(projectileHitbox, _roaches[i].hitbox)) {
                    if (playerProjectiles.projectiles[j].type & FREEZING_PROJECTILE) {
                        _roaches[i].speed = enemySpeed / 2;
                    }
                    else {
                        _roaches[i].speed = enemySpeed;
                    }
                    if (enemy_take_damage(&_roaches[i], projectile.dmg)) {
                        _deadEnemies++;
                        // Base exp on this formula:
                        // (max health / speed) * (max enemies / 10)
                        playerInstance.expGained += _get_exp(_roaches[i].maxHealth, _roaches[i].speed, _roachesMax);
                        memset(&_roaches[i], 0, sizeof(_roaches[i]));
                    }
                    remove_projectile(&playerProjectiles, j);
                    break;
                }
                if (playerProjectiles.projectiles[j].type & TRACKING_PROJECTILE) {
                    struct Circle radarBullet = { .pos = playerProjectiles.projectiles[j].pos, .diameter = 2 * fabs(playerInstance.width) }; // define bullet radar
                    if (are_circle_rect_intersecting(radarBullet, _roaches[i].hitbox)) {
                        playerProjectiles.projectiles[j].shootDirection = CP_Vector_Normalize(CP_Vector_Subtract(_roaches[i].hitbox.pos, playerProjectiles.projectiles[j].pos));
                    }
                }
            }

            // Boss
            if (are_circle_rect_intersecting(projectileHitbox, _boss.hitbox)) {
                if (playerProjectiles.projectiles[j].type & FREEZING_PROJECTILE) {
                    _boss.speed = bossSpeed / 2;
                }
                else {
                    _boss.speed = bossSpeed;
                }
                if (boss_take_damage(&_boss, projectile.dmg)) {
                    _deadEnemies++;
                    float expGained = _get_exp(_boss.maxHealth, _boss.speed, 1) * 12.f;
                    playerInstance.expGained += expGained;
                    dispose_boss(&_boss);
                }
                remove_projectile(&playerProjectiles, j);
                continue;
            }
            if (playerProjectiles.projectiles[j].type & TRACKING_PROJECTILE) {
                struct Circle radarBullet = { .pos = playerProjectiles.projectiles[j].pos, .diameter = 2 * fabs(playerInstance.width) }; // define bullet radar
                if (are_circle_rect_intersecting(radarBullet, _boss.hitbox)) {
                    playerProjectiles.projectiles[j].shootDirection = CP_Vector_Normalize(CP_Vector_Subtract(_boss.hitbox.pos, playerProjectiles.projectiles[j].pos));
                }
            }
        }
    }

    // Ally Projectiles
    for (int i = 0; i < allyCount; i++) {
        for (int j = 0; j < allyProjectiles[i].activeCount; j++) {
            if (allyProjectiles[i].projectiles[j].isActive) {
                struct Projectile projectile = allyProjectiles[i].projectiles[j];
                struct Circle projectileHitbox = { .pos = projectile.pos, .diameter = projectile.diameter };

                // Obstacles
                for (int x = 0; x < _worldClipsCount; x++) {
                    struct Rectangle worldClip = _worldClips[x];
                    if (are_circle_rect_intersecting(projectileHitbox, worldClip)) {// bullet cannot go through walls and barricades...
                        remove_projectile(&allyProjectiles[i], j);
                        break;
                    }
                }

                // Common Enemies (roaches)
                for (int x = 0; x < _roachesMax; x++) {
                    if (are_circle_rect_intersecting(projectileHitbox, _roaches[x].hitbox)) {
                        if (enemy_take_damage(&_roaches[x], projectile.dmg)) {
                            _deadEnemies++;
                            // Base exp on this formula:
                            // (max health / speed) * (max enemies / 10)
                            playerInstance.expGained += _get_exp(_roaches[x].maxHealth, _roaches[x].speed, _roachesMax);
                            memset(&_roaches[x], 0, sizeof(_roaches[x]));
                        }
                        remove_projectile(&allyProjectiles[i], j);
                        break;
                    }
                }

                // Boss
                if (are_circle_rect_intersecting(projectileHitbox, _boss.hitbox)) {
                    if (boss_take_damage(&_boss, projectile.dmg)) {
                        _deadEnemies++;
                        float expGained = _get_exp(_boss.maxHealth, _boss.speed, 1) * 12.f;
                        playerInstance.expGained += expGained;
                        dispose_boss(&_boss);
                    }
                    remove_projectile(&allyProjectiles[i], j);
                    continue;
                }
            }
        }
    }
}

void enemy_move_attack(float dt) {
    if (isPaused || isWin) return;

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
        if (_roaches[i].barrierAttack < MAX_BARRIERS)
            continue;

        struct Circle radarRoach = { .pos = _roaches[i].pos, .diameter = 5 * fabs(_roaches[i].width) }; // define enemy radar
        if (are_circle_rect_intersecting(radarRoach, playerInstance.hitbox)) { // Check enemy near with player
            _roaches[i].currentState = ENEMY_STATE_MOVING_TO_PLAYER;
            continue;
        }
        _roaches[i].currentState = ENEMY_STATE_MOVING_TO_TOWER;
    }
    #pragma endregion

    bool isBarrierDestroyed = false;
    for (int i = 0; i < _roachesMax; i++) {
        if (_roaches[i].currentState) { // if enemy is alive
            #pragma region Enemy Movement & Attack 
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
                            if (playerInstance.isReflectingDmg) {
                                if (_roaches[i].attackCooldown <= 0) {
                                    _roaches[i].attackCooldown = _enemyMaxAttackCD;
                                    if (enemy_take_damage(&_roaches[i], _roaches[i].dmg)) {
                                        _deadEnemies++;
                                        // Base exp on this formula:
                                        // (max health / speed) * (max enemies / 10)
                                        playerInstance.expGained += _get_exp(_roaches[i].maxHealth, _roaches[i].speed, _roachesMax);
                                        memset(&_roaches[i], 0, sizeof(_roaches[i]));
                                    }
                                }
                                break;
                            }
                            player_take_damage(&playerInstance, _roaches[i].dmg);
                            break;
                
                        case TARGET_TOWER:
                            tower_take_damage(&_tower, 1);
                            break;
                
                        case TARGET_BARRIER: 
                            if (_roaches[i].attackCooldown <= 0) {
                                _roaches[i].attackCooldown = _enemyMaxAttackCD;
                                barrier_take_damage(_roaches[i].barrierAttack, _roaches[i].dmg);
                            }
                            _roaches[i].barrierAttack = MAX_BARRIERS;
                            break;
                    }
                    break;
                }

                case ENEMY_STATE_IDLE:
                    _roaches[i].moveDirection = CP_Vector_Zero();
                    break;
            }
            CP_Vector hitboxPos = { _roaches[i].pos.x + ENEMY_HITBOX_OFFSET_X(_roaches[i].width), _roaches[i].pos.y + ENEMY_HITBOX_OFFSET_Y(_roaches[i].height) };
            update_rect_hitbox(&_roaches[i].hitbox, 0.0f, 0.0f, hitboxPos); // Update hitbox of enemy
            #pragma endregion
        }
        // can use else statement here for dying animation...
    }

    #pragma region Boss Movement & Attack
    switch (_boss.state) {
    case BOSS_STATE_MOVING_TO_TOWER:
        _boss.move(&_boss, playerInstance, get_tower_hitbox(_tower), dt, _bossGrid.nodes, _bossGridNB.nodes, _bossGrid.nodeCount);
        break;

    case BOSS_STATE_MOVING_TO_PLAYER:
        _boss.move(&_boss, playerInstance, playerInstance.hitbox, dt, _bossGrid.nodes, _bossGridNB.nodes, _bossGrid.nodeCount);
        break;

    case BOSS_STATE_IDLE:
        _boss.moveDirection = CP_Vector_Zero();
        break;
    }

    if (_boss.canPrimaryAttack(_boss, playerInstance, _tower))
        _boss.primaryAttack(&_boss, &playerInstance, &_tower, dt);
    else if (_boss.canSecondaryAttack(_boss, playerInstance))
        _boss.secondaryAttack(&_boss, &playerInstance, &_tower, dt);
    #pragma endregion
}

void buff_update(float dt) {
    if (isPaused || isWin) return;
    BuffType randomBuff = update_buff_meter(&playerInstance, dt);
    // Apply the buff if it is upgrades buff
    if (randomBuff) {
        if (randomBuff >= ATTACK_UP && randomBuff <= BARRIER_LVL_UP) {
            apply_buff(&activeBuffs, randomBuff, &playerInstance, barriers, allies);
        }
        else {
            //active buff
            if (buffSlot.buffCount < MAX_BUFF_SLOT) {
                assign_buff_to_slot(&buffSlot, randomBuff);
            }
            else {
                tempSlot = randomBuff;
            }
        }
    }

    if (tempSlot && buffSlot.buffCount < MAX_BUFF_SLOT) {
        assign_buff_to_slot(&buffSlot, tempSlot);
        tempSlot = NO_BUFF;
    }

    if (CP_Input_KeyTriggered(KEY_1) && buffSlot.buffs[0] != NO_BUFF) {
        apply_buff(&activeBuffs, buffSlot.buffs[0], &playerInstance, barriers, allies);
        buffSlot.buffs[0] = NO_BUFF;
        buffSlot.buffCount--;
    }
    if (CP_Input_KeyTriggered(KEY_2) && buffSlot.buffs[1] != NO_BUFF) {
        apply_buff(&activeBuffs, buffSlot.buffs[1], &playerInstance, barriers, allies);
        buffSlot.buffs[1] = NO_BUFF;
        buffSlot.buffCount--;
    }
    if (CP_Input_KeyTriggered(KEY_3) && buffSlot.buffs[2] != NO_BUFF) {
        apply_buff(&activeBuffs, buffSlot.buffs[2], &playerInstance, barriers, allies);
        buffSlot.buffs[2] = NO_BUFF;
        buffSlot.buffCount--;
    }

    update_timer_buff(&activeBuffs, dt);
}

void place_barrier(int index, float x, float y) {
    set_barrier_properties(index, CP_Vector_Set(x, y), BARRIER_WIDTH, BARRIER_HEIGHT, BARRIER_INITIAL_HEALTH);
    increment_barrier_count();

    _worldClips[_worldClipsCount++] = barriers[index].hitbox;
    update_grid(&_enemyGrid, _worldClips, _worldClipsCount, _enemyClips, 4);
    update_grid(&_bossGrid, _worldClips, _worldClipsCount, _enemyClips, 4);
}

void place_ally(int index, float x, float y) {
    init_ally(index, CP_Vector_Set(x, y), ALLY_WIDTH, ALLY_HEIGHT);
    increment_ally_count();
}

#if _DEBUG
void debug_render_enemy_grid(void) {
    draw_grid(_enemyGrid.nodes, _enemyGrid.nodeCount); // draw enemy grid for debugging
    draw_grid(_enemyGridNB.nodes, _enemyGrid.nodeCount); // draw enemy grid for debugging
}

void debug_render_boss_grid(void) {
     draw_grid(_bossGrid.nodes, _bossGrid.nodeCount); // draw boss grid for debugging
     draw_grid(_bossGridNB.nodes, _bossGrid.nodeCount); // draw boss grid for debugging
}

void debug_render_clips(void) {
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 125));
    for (int i = 0; i < _worldClipsCount; i++)
        CP_Graphics_DrawRect(_worldClips[i].pos.x, _worldClips[i].pos.y, _worldClips[i].width, _worldClips[i].height);

    for (int x = 0; x < 4; x++)
        CP_Graphics_DrawRect(_enemyClips[x].pos.x, _enemyClips[x].pos.y, _enemyClips[x].width, _enemyClips[x].height);
}
#endif

void dispose_resources(void) {
    if (!_isMovingToNextLevel)
        CP_Image_Free(&playerInstance.img);
    CP_Image_Free(&_heartImg);
    CP_Image_Free(&_playerPortraitImg);
    CP_Image_Free(&_tower.img);
    
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
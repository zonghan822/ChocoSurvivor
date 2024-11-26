#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "cprocessing.h"
#include "mainmenu.h"
#include "level.h"
#include "level1.h"
#include "level2.h"
#include "camera.h"
#include "hittest.h"
#include "utils.h"
#include "audio.h"
#include "gui.h"
#include "projectile.h"
#include "player.h"
#include "placeables.h"

#define LEVEL_WIDTH 2500.f
#define LEVEL_HEIGHT 900.f

static CP_Image _cursor, _levelBg;

static float _windowWidth, _windowHeight;

// Tutorial input variables (private)
static CP_Image _tutImageW, _tutImageA, _tutImageS, _tutImageD;
static CP_Image _tutImageSpace;
static CP_Image _tutImageV, _tutImageB;
static CP_Image _tutImageRmb, _tutImageLmb;
static bool _tutHasPressedW, _tutHasPressedA, _tutHasPressedS, _tutHasPressedD; // WASD
static bool _tutHasPressedSpace; // Space
static bool _tutHasPressedV, _tutHasPressedB; // Ally and Barrier, respectively
static bool _tutHasPressedRmb, _tutHasHoldLmb;
static float _tutBarrierX, _tutBarrierY;
static float _tutAllyX, _tutAllyY;
static bool _hasPressedF11, _hasPressedF12;
static float _tutLmbHoldTime;

static void _render_tutorials(float dt) {
    CP_Settings_StrokeWeight(5.f);
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
    float square = 50.f,
        cornerRadius = 5.f;
    // WASD
    {
        if (!_tutHasPressedW) { // W
            float x = playerInstance.pos.x - 20.f,
                y = playerInstance.pos.y - 140.f;

            CP_Graphics_DrawRectAdvanced(x, y, square - 14.f, square - 14.f, 0.f, cornerRadius);
            CP_Image_Draw(_tutImageW,
                x, y,
                square, square,
                255);

            _tutHasPressedW = CP_Input_KeyTriggered(KEY_W);
        }
        if (!_tutHasPressedA) { // A
            float x = playerInstance.pos.x - 70.f,
                y = playerInstance.pos.y - 90.f;

            CP_Graphics_DrawRectAdvanced(x, y, square - 14.f, square - 14.f, 0.f, cornerRadius);
            CP_Image_Draw(_tutImageA,
                x, y,
                square, square,
                255);

            _tutHasPressedA = CP_Input_KeyTriggered(KEY_A);
        }
        if (!_tutHasPressedS) { // S
            float x = playerInstance.pos.x - 20.f,
                y = playerInstance.pos.y - 90.f;

            CP_Graphics_DrawRectAdvanced(x, y, square - 14.f, square - 14.f, 0.f, cornerRadius);
            CP_Image_Draw(_tutImageS,
                x, y,
                square, square,
                255);

            _tutHasPressedS = CP_Input_KeyTriggered(KEY_S);
        }
        if (!_tutHasPressedD) { // D
            float x = playerInstance.pos.x + 30.f,
                y = playerInstance.pos.y - 90.f;

            CP_Graphics_DrawRectAdvanced(x, y, square - 14.f, square - 14.f, 0.f, cornerRadius);
            CP_Image_Draw(_tutImageD,
                x, y,
                square, square,
                255);

            _tutHasPressedD = CP_Input_KeyTriggered(KEY_D);
        }
    }

    // Spacebar
    {
        if (_tutHasPressedW && _tutHasPressedA && _tutHasPressedS && _tutHasPressedD && !_tutHasPressedSpace) {
            float x = playerInstance.pos.x - 20.f,
                y = playerInstance.pos.y - 90.f;

            float width = 55.f;
            float imgWidth = (float)CP_Image_GetWidth(_tutImageSpace),
                imgHeight = (float)CP_Image_GetHeight(_tutImageSpace);
            float height = get_preserved_heightf(imgWidth, imgHeight, width);
            CP_Settings_TextSize(35.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Font_DrawTextBox("Press         to dash", x - 135.f, y + 5.f, 255.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRectAdvanced(x - 20.f, y, width - 14.f, height - 31.f, 0.f, cornerRadius + 1.f);
            CP_Image_Draw(_tutImageSpace,
                x - 20.f, y,
                width, height,
                255);

            _tutHasPressedSpace = CP_Input_KeyTriggered(KEY_SPACE);
        }
    }

    // Hold Mouse Button
    {
        if (_tutHasPressedW && _tutHasPressedA && _tutHasPressedS && _tutHasPressedD && _tutHasPressedSpace && !_tutHasHoldLmb) {
            float x = playerInstance.pos.x - 20.f,
                y = playerInstance.pos.y - 90.f;

            float width = 55.f;
            float imgWidth = (float)CP_Image_GetWidth(_tutImageLmb),
                imgHeight = (float)CP_Image_GetHeight(_tutImageLmb);
            float height = get_preserved_heightf(imgWidth, imgHeight, width);
            CP_Settings_TextSize(35.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Font_DrawTextBox("Hold      to shoot", x - 135.f, y + 5.f, 255.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRectAdvanced(x - 35.f, y, 32.f, 36.f, 0.f, 5.f);
            CP_Image_Draw(_tutImageLmb,
                x - 35.f, y,
                50.f, 50.f,
                255);
            if (CP_Input_MouseDown(MOUSE_BUTTON_LEFT)) {
                _tutLmbHoldTime += dt;
            }
            _tutHasHoldLmb = _tutLmbHoldTime >= 2.f;
            if (CP_Input_MouseReleased(MOUSE_BUTTON_LEFT)) {
                _tutLmbHoldTime = 0;
            }
        }
    }

    // Placeables
    {
        if (!_tutHasPressedB) { // B
            float x = _tutBarrierX + 15.f,
                y = _tutBarrierY - 140.f;

            CP_Settings_TextSize(35.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Font_DrawTextBox("Press        to place barricades", x - 135.f, y + 5.f, 225.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRectAdvanced(x, y, square - 14.f, square - 14.f, 0.f, cornerRadius);
            CP_Image_Draw(_tutImageB,
                x, y,
                square, square,
                255);

            _tutHasPressedB = CP_Input_KeyTriggered(KEY_B);
        }
        if (!_tutHasPressedV) { // V
            float x = _tutAllyX + 15.f,
                y = _tutAllyY - 140.f;

            CP_Settings_TextSize(35.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Font_DrawTextBox("Press        to place sentries", x - 135.f, y + 5.f, 225.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRectAdvanced(x, y, square - 14.f, square - 14.f, 0.f, cornerRadius);
            CP_Image_Draw(_tutImageV,
                x, y,
                square, square,
                255);

            _tutHasPressedV = CP_Input_KeyTriggered(KEY_V);
        }
    }
    CP_Settings_StrokeWeight(3.f);
}

static bool _tutCanCancelPlaceablesRmb;

static void _render_mouse_tutorials() {
    CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));

    if (_tutCanCancelPlaceablesRmb && (CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT) || CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))) {
        _tutHasPressedRmb = CP_Input_MouseTriggered(MOUSE_BUTTON_RIGHT) || CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT);
        _tutCanCancelPlaceablesRmb = false;
    }

    // Mouse Buttons
    {
        // Show RMB to cancel placeables when placeable key inputs are pressed
        if ((!_tutHasPressedRmb && (CP_Input_KeyTriggered(KEY_V) || CP_Input_KeyTriggered(KEY_B))) || _tutCanCancelPlaceablesRmb) {
            float x = CP_Input_GetMouseX() + 15.f,
                y = CP_Input_GetMouseY() - 140.f;

            CP_Settings_TextSize(35.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
            CP_Font_DrawTextBox("Press        to cancel", x - 135.f, y + 5.f, 225.f);
            CP_Settings_Fill(CP_Color_Create(0, 0, 0, 100));
            CP_Graphics_DrawRectAdvanced(x, y, 32.f, 36.f, 0.f, 5.f);
            CP_Image_Draw(_tutImageRmb,
                x, y,
                50.f, 50.f,
                255);

            _tutCanCancelPlaceablesRmb = true;
        }
    }
}

static void _debug_render_grid() {
    if (CP_Input_KeyTriggered(KEY_F11)) {
        _hasPressedF11 = !_hasPressedF11;
        _hasPressedF12 = false;
    }
    if (CP_Input_KeyTriggered(KEY_F12)) {
        _hasPressedF12 = !_hasPressedF12;
        _hasPressedF11 = false;
    }
    if (_hasPressedF11) debug_render_enemy_grid();
    if (_hasPressedF12) debug_render_boss_grid();
     //debug_render_clips();
}

void level_one_init(void) {
    init_tilemap();
    _cursor = CP_Image_Load("Assets/cursor.png");
    CP_System_ShowCursor(FALSE);

    _windowWidth = (float)CP_System_GetWindowWidth();
    _windowHeight = (float)CP_System_GetWindowHeight();

    _levelBg = init_level(GAME_LEVEL_ONE, LEVEL_WIDTH, LEVEL_HEIGHT, _windowWidth, _windowHeight);
    _tutHasPressedW = _tutHasPressedA = _tutHasPressedS =
        _tutHasPressedD = _tutHasPressedV = _tutHasPressedB =
        _tutHasPressedRmb = _tutHasHoldLmb =
        _tutCanCancelPlaceablesRmb = _tutHasPressedSpace = 
        _hasPressedF11 = _hasPressedF12 = false;
    
    _tutImageW = CP_Image_Load("Assets/GUI/keyboard_w.png");
    _tutImageA = CP_Image_Load("Assets/GUI/keyboard_a.png");
    _tutImageS = CP_Image_Load("Assets/GUI/keyboard_s.png");
    _tutImageD = CP_Image_Load("Assets/GUI/keyboard_d.png");
    _tutImageV = CP_Image_Load("Assets/GUI/keyboard_v.png");
    _tutImageB = CP_Image_Load("Assets/GUI/keyboard_b.png");
    _tutImageRmb = CP_Image_Load("Assets/GUI/mouse_right.png");
    _tutImageLmb = CP_Image_Load("Assets/GUI/mouse_left.png");
    _tutImageSpace = CP_Image_Load("Assets/GUI/keyboard_space.png");

    // Place tutorial placeables
    _tutBarrierX = 895.f;
    _tutBarrierY = 645.f;
    _tutAllyX = 650.f;
    _tutAllyY = 275.f;
    _tutLmbHoldTime = 0.f;
    place_barrier(0, _tutBarrierX, _tutBarrierY);
    place_ally(0, _tutAllyX, _tutAllyY);

    init_sounds();
    play_sound_advanced(SOUND_MUSIC_NEW_MAP_LEVEL, 1.f, 0);
}

void level_one_update(void) {
    // PLACEHOLDER
    // TODO: REMOVE
    /*if (CP_Input_KeyTriggered(KEY_O)) {
        isMovingToNextLevel = true;
        CP_Engine_SetNextGameState(level_two_init, level_two_update, level_two_exit);
    }*/

    float dt = CP_System_GetDt();
    float mouseWorldX = CP_Input_GetMouseWorldX(),
        mouseWorldY = CP_Input_GetMouseWorldY();
    float mouseX = CP_Input_GetMouseX(),
        mouseY = CP_Input_GetMouseY();

    if (try_game_over(_windowWidth, _windowHeight, _cursor))
        return; // Game over, so don't do anything else

    // Function to TRY to proceed to next wave or level
    // Function is slightly hardcoded
    next_wave_or_level();

    if (!isPaused && !isWin) {
        int worldClipsCount = 0;
        const struct Rectangle* worldClips = get_world_clips(&worldClipsCount);
        move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT, worldClips, worldClipsCount);
    }
    update_camera(playerInstance, dt);
   
    // Render background
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    CP_Image_Draw(_levelBg, (LEVEL_WIDTH / 2.f), (LEVEL_HEIGHT / 2.f), LEVEL_WIDTH, LEVEL_HEIGHT, 255);

    enableShooting = !isPaused && !isWin;

    // Spawn enemies
    spawn_enemies(dt);

    // Player placeables
    player_placeables(mouseWorldX, mouseWorldY, dt);

    /*********************************
           Rendering with camera
    **********************************/
    _debug_render_grid();
    render_barriers();
    render_allies();
    render_entities(dt, mouseWorldX);
    render_health_bars(dt);
    _render_tutorials(dt);

    /*********************************
         Movement and Attack Logic
    **********************************/
    if (!isPaused && !isWin)
        player_shoot(dt, enableShooting, mouseWorldX, mouseWorldY);
    check_collision(); // For player
    enemy_move_attack(dt);
    buff_update(dt);

    /*********************************
         Rendering without camera
    **********************************/
    CP_Settings_ResetMatrix();
    render_new_wave_counter(_windowWidth, _windowHeight);
    render_ui(_windowWidth, _windowHeight, mouseX, mouseY, dt);
    try_pause(_windowWidth, _windowHeight);
    _render_mouse_tutorials();
    _render_victory_page(_windowWidth, _windowHeight);

    render_cursor(_cursor);
    apply_camera();

}

void level_one_exit(void) {
    CP_Image_Free(&_cursor);
    dispose_sounds();
    dispose_tilemap();
    dispose_resources();

    CP_Image_Free(&_tutImageW);
    CP_Image_Free(&_tutImageA);
    CP_Image_Free(&_tutImageS);
    CP_Image_Free(&_tutImageD);
    CP_Image_Free(&_tutImageV);
    CP_Image_Free(&_tutImageB);
    CP_Image_Free(&_tutImageRmb);
    CP_Image_Free(&_tutImageSpace);
}
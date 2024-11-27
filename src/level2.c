#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#include "cprocessing.h"
#include "mainmenu.h"
#include "level1.h"
#include "level2.h"
#include "player.h"
#include "enemy.h"
#include "tower.h"
#include "camera.h"
#include "hittest.h"
#include "utils.h"
#include "flicker.h"
#include "placeables.h" 
#include "astar.h"
#include "boss.h"
#include "button.h"
#include "audio.h"
#include "gui.h"
#include "level.h"

#define LEVEL_WIDTH 2400.f
#define LEVEL_HEIGHT 900.f

CP_Image _cursor, _levelBg;

float _windowWidth, _windowHeight;

void level_two_init(void) {
    init_tilemap();
    _cursor = CP_Image_Load("Assets/GUI/Icons/cursor.png");
    CP_System_ShowCursor(FALSE);

    _windowWidth = (float)CP_System_GetWindowWidth();
    _windowHeight = (float)CP_System_GetWindowHeight();

    _levelBg = init_level(GAME_LEVEL_TWO, LEVEL_WIDTH, LEVEL_HEIGHT, _windowWidth, _windowHeight);

    init_sounds();
    play_sound_advanced(SOUND_MUSIC_NEW_MAP_LEVEL, 1.f, 0);
}

void level_two_update(void) {
    // PLACEHOLDER
    // TODO: REMOVE
    /*if (CP_Input_KeyTriggered(KEY_O)) {
        isMovingToNextLevel = true;
        CP_Engine_SetNextGameState(level_two_init, level_two_update, level_two_exit);
    }*/

    float dt = CP_System_GetDt();
    float mouseX = CP_Input_GetMouseWorldX(),
        mouseY = CP_Input_GetMouseWorldY();

    if (try_game_over(_windowWidth, _windowHeight, _cursor))
        return; // Game over, so don't do anything else

    // move_player(&playerInstance, dt, LEVEL_WIDTH, LEVEL_HEIGHT);

    // Function to TRY to proceed to next wave or level
    // Function is slightly hardcoded
    next_wave_or_level();

    // Render background
    CP_Graphics_ClearBackground(CP_Color_Create(170, 170, 170, 255));
    CP_Image_Draw(_levelBg, (LEVEL_WIDTH / 2.f), (LEVEL_HEIGHT / 2.f), LEVEL_WIDTH, LEVEL_HEIGHT, 255);

    enableShooting = true;

    // Spawn enemies
    spawn_enemies(dt);

    // Player placeables
    player_placeables(mouseX, mouseY, dt);

    /*********************************
                 Rendering
    **********************************/
    render_barriers();
    render_allies();
    render_entities(dt, mouseX);
    render_health_bars(dt);

    /*********************************
         Movement and Attack Logic
    **********************************/
    player_shoot(dt, enableShooting, mouseX, mouseY);
    check_collision(); // For player
    enemy_move_attack(dt);

    // Render any UI here
    CP_Settings_ResetMatrix();
    render_new_wave_counter(_windowWidth, _windowHeight);
    render_ui(_windowWidth, _windowHeight, mouseX, mouseY, dt);
    render_cursor(_cursor);
    apply_camera();

}

void level_two_exit(void) {
    CP_Image_Free(&_cursor);
    dispose_sounds();
    dispose_tilemap();
    dispose_resources();
}
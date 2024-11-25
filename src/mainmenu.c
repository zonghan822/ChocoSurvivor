#include "mainmenu.h"
#include "level1.h"
#include "level2.h"
#include "hittest.h"
#include "cprocessing.h"
#include "audio.h"
#include "button.h"
#include "panel.h"
#include "slider.h"
#include "progressbar.h"
#include "gui.h"
#include "optionsmenu.h"

#include <stdio.h>

static struct Button _playButton, _optionsButton, _quitButton;
static CP_Image _cursor;
static CP_Color _bg;

static float _windowWidth, _windowHeight;
static float _buttonRectX, _buttonRectY, _buttonRectHeight, _buttonRectWidth;

void main_menu_init(void) {
	init_sounds();
	init_tilemap();
	_cursor = CP_Image_Load("Assets/cursor.png");
	CP_System_ShowCursor(false);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/Tiny5/Tiny5-Regular.ttf"));

	_windowWidth = CP_System_GetWindowWidth();
	_windowHeight = CP_System_GetWindowHeight();

	_bg = CP_Color_Create(128, 128, 128, 255);

	_buttonRectX = _windowWidth / 2.0f;
	_buttonRectY = _windowHeight / 2.0f;
	_buttonRectHeight = 85.0f;
	_buttonRectWidth = 300.0f;

	CP_Color white = CP_Color_Create(255, 255, 255, 255);
	_playButton = init_button(_buttonRectX, _buttonRectY - (_buttonRectHeight * 2.f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_SUCCESS,
		"Play", CP_Color_Create(0, 0, 0, 255), 55.f);

	_optionsButton = init_button(_buttonRectX, _buttonRectY, _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_DEFAULT,
		"Options", white, 55.f);

	_quitButton = init_button(_buttonRectX, _buttonRectY + (_buttonRectHeight * 2.f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_WARNING,
		"Quit", white, 55.f);
}

void main_menu_update(void) {
	CP_Graphics_ClearBackground(_bg);

	CP_Settings_TextSize(125.0f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Font_DrawText("CHOCO SURVIVOR", _buttonRectX, _windowHeight / 6.5f);

	float mouseX = CP_Input_GetMouseWorldX(),
		mouseY = CP_Input_GetMouseWorldY();

	render_button(_playButton);
	render_button(_optionsButton);
	render_button(_quitButton);
	if (!is_options_menu_open()) {
		update_button_state(&_playButton, mouseX, mouseY);
		update_button_state(&_optionsButton, mouseX, mouseY);
		update_button_state(&_quitButton, mouseX, mouseY);
	}

	// Options menu is open
	if (is_options_menu_open()) {
		render_options_menu(mouseX, mouseY, (float)_windowWidth, (float)_windowHeight);
	}
	else {
		// Play
		if (_playButton.state == BUTTON_STATE_CLICKED)
			CP_Engine_SetNextGameState(level_one_init, level_one_update, level_one_exit);

		// Options
		if (_optionsButton.state == BUTTON_STATE_CLICKED)
			render_options_menu(mouseX, mouseY, (float)_windowWidth, (float)_windowHeight);

		// Maybe Credits?

		// Quit
		if (_quitButton.state == BUTTON_STATE_CLICKED)
			CP_Engine_Terminate();
	}

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	float cursorWidth = 60.f;
	float cursorImgWidth = ((float)CP_Image_GetWidth(_cursor)),
		cursorImgHeight = ((float)CP_Image_GetHeight(_cursor));
	// 20.f is to correct its draw position with the actual cursor position
	CP_Image_Draw(_cursor, mouseX - 20.f, mouseY, cursorWidth,
		get_preserved_heightf(cursorImgWidth, cursorImgHeight, cursorWidth), 255);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
}

void main_menu_exit(void) {
	CP_Image_Free(&_cursor);

	dispose_sounds();
	dispose_tilemap();
}
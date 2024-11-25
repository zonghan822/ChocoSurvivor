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
#include "creditsmenu.h"

#include <stdio.h>

static struct Button _playButton, _optionsButton, _creditsButton, _quitButton;
static CP_Image _cursor;

// DigiPen logo
// Show only once, so it doesn't show again after going back to menu
static float _digiPenLogoTimer = 0.f;
static CP_Image _digiPenLogoImg;
static float _digiPenLogoWidth = 0.f;
static int _digiPenLogoAlpha = 255, _digiPenScreenAlpha = 255;

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

	_windowWidth = (float)CP_System_GetWindowWidth();
	_windowHeight = (float)CP_System_GetWindowHeight();

	_buttonRectX = _windowWidth / 2.f;
	_buttonRectY = _windowHeight / 2.f;
	_buttonRectHeight = 85.f;
	_buttonRectWidth = 300.f;

	_digiPenLogoImg = CP_Image_Load("Assets/DigiPen_Singapore_WEB_WHITE.png");

	CP_Color white = CP_Color_Create(255, 255, 255, 255);
	_playButton = init_button(_buttonRectX, _buttonRectY - (_buttonRectHeight * 1.75f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_SECONDARY,
		"Play", CP_Color_Create(0, 0, 0, 255), 55.f);

	_optionsButton = init_button(_buttonRectX, _buttonRectY - (_buttonRectHeight * 0.25f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_PRIMARY,
		"Options", white, 55.f);

	_creditsButton = init_button(_buttonRectX, _buttonRectY + (_buttonRectHeight * 1.25f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_PRIMARY,
		"Credits", white, 55.f);

	_quitButton = init_button(_buttonRectX, _buttonRectY + (_buttonRectHeight * 2.75f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_WARNING,
		"Quit", white, 55.f);
}

void main_menu_update(void) {
	float dt = CP_System_GetDt();

	const int startAlpha = 255;
	const int endAlpha = 0;

	if (_digiPenLogoTimer <= 4.f) {
		CP_Graphics_ClearBackground(CP_Color_Create(0, 0, 0, 255));
		float digiPenLogoWidth = (float)CP_Image_GetWidth(_digiPenLogoImg);
		float digiPenLogoHeight = (float)CP_Image_GetHeight(_digiPenLogoImg);

		_digiPenLogoWidth += (_digiPenLogoWidth == 0.f)
			? digiPenLogoWidth / 2.f
			: 22.5f * dt;

		// Lerp alpha to 0
		// 4.f because it takes 4 seconds (4.f) before forgetting about this code
		_digiPenLogoAlpha = CP_Math_LerpInt(startAlpha, endAlpha, _digiPenLogoTimer / 4.f);

		float preservedHeight = get_preserved_heightf(digiPenLogoWidth, digiPenLogoHeight, _digiPenLogoWidth);
		CP_Image_Draw(_digiPenLogoImg,
			_windowWidth / 2.f, _windowHeight / 2.f,
			_digiPenLogoWidth, preservedHeight,
			_digiPenLogoAlpha);

		CP_Settings_TextSize(30.0f);
		CP_Font_Set(CP_Font_Load("Assets/Fonts/PixelifySans/PixelifySans-VariableFont_wght.ttf"));
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, _digiPenLogoAlpha));
		CP_Font_DrawText("All content \xC2\xA9 2024 DigiPen Institute of Technology Singapore. All Rights Reserved", 
			_windowWidth / 2.f, _windowHeight - 25.f);
		CP_Font_Set(CP_Font_Load("Assets/Fonts/Tiny5/Tiny5-Regular.ttf"));

		_digiPenLogoTimer += dt;
		return;
	}

	CP_Graphics_ClearBackground(CP_Color_Create(128, 128, 128, 255));

	CP_Settings_TextSize(125.0f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Font_DrawText("CHOCO SURVIVOR", _buttonRectX, _windowHeight / 6.5f);

	float mouseX = CP_Input_GetMouseWorldX(),
		mouseY = CP_Input_GetMouseWorldY();

	render_button(_playButton);
	render_button(_optionsButton);
	render_button(_creditsButton);
	render_button(_quitButton);
	if (!is_options_menu_open() && !is_credits_menu_open()) {
		update_button_state(&_playButton, mouseX, mouseY);
		update_button_state(&_optionsButton, mouseX, mouseY);
		update_button_state(&_creditsButton, mouseX, mouseY);
		update_button_state(&_quitButton, mouseX, mouseY);
	}

	// Options menu is open
	if (is_options_menu_open()) {
		render_options_menu(mouseX, mouseY, (float)_windowWidth, (float)_windowHeight);
	}
	else if (is_credits_menu_open()) {
		render_credits_menu(mouseX, mouseY, (float)_windowWidth, (float)_windowHeight);
	}
	else {
		// Play
		if (_playButton.state == BUTTON_STATE_CLICKED)
			CP_Engine_SetNextGameState(level_one_init, level_one_update, level_one_exit);

		// Options
		if (_optionsButton.state == BUTTON_STATE_CLICKED)
			render_options_menu(mouseX, mouseY, (float)_windowWidth, (float)_windowHeight);

		// Credits
		if (_creditsButton.state == BUTTON_STATE_CLICKED)
			render_credits_menu(mouseX, mouseY, (float)_windowWidth, (float)_windowHeight);

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

	if (_digiPenLogoTimer <= 4.5f) {
		// - 4.f because the logo takes 4 seconds
		// 0.5f because 4.5f - 4.f = 0.5f (this is the remainder)
		_digiPenScreenAlpha = CP_Math_LerpInt(startAlpha, endAlpha, (_digiPenLogoTimer - 4.f) / 0.5f);

		CP_Settings_StrokeWeight(0.f);
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, _digiPenScreenAlpha));
		CP_Graphics_DrawRect(_windowWidth / 2.f, _windowHeight / 2.f, _windowWidth, _windowHeight);

		_digiPenLogoTimer += dt;
		CP_Settings_StrokeWeight(3.f);
	}
}

void main_menu_exit(void) {
	CP_Image_Free(&_cursor);
	CP_Image_Free(&_digiPenLogoImg);
	_digiPenLogoTimer = 4.51f;

	dispose_sounds();
	dispose_tilemap();
}
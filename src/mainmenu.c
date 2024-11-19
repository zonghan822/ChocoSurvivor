#include "mainmenu.h"
#include "level1.h"
#include "level2.h"
#include "hittest.h"
#include "cprocessing.h"
#include "audio.h"
#include "button.h"
#include "panel.h"
#include "slider.h"

#include <stdio.h>

struct Button playButton, optionsButton, quitButton;
struct Panel panel;
struct Slider slider;
CP_Image cursor;
CP_Color bg;

static int _windowWidth, _windowHeight;
static float _buttonRectX, _buttonRectY, _buttonRectHeight, _buttonRectWidth;

void main_menu_init(void){
	init_sounds();
	cursor = CP_Image_Load("Assets/cursor.png");
	CP_System_ShowCursor(FALSE);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

	_windowWidth = CP_System_GetWindowWidth();
	_windowHeight = CP_System_GetWindowHeight();

	bg = CP_Color_Create(128, 128, 128, 255);

	_buttonRectX = _windowWidth / 2.0f;
	_buttonRectY = _windowHeight / 2.0f;
	_buttonRectHeight = 85.0f;
	_buttonRectWidth = 300.0f;

	playButton = init_button_image(_buttonRectX, _buttonRectY - (_buttonRectHeight * 2.f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_SUCCESS,
		"Play", _buttonRectX, _buttonRectY - (_buttonRectHeight * 2.f) + 5.f, CP_Color_Create(0, 0, 0, 255), 55.f);

	optionsButton = init_button_image(_buttonRectX, _buttonRectY, _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_DEFAULT,
		"Options", _buttonRectX, _buttonRectY + 5.f, CP_Color_Create(0, 0, 0, 255), 55.f);

	quitButton = init_button_image(_buttonRectX, _buttonRectY + (_buttonRectHeight * 2.f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_WARNING,
		"Quit", _buttonRectX, _buttonRectY + (_buttonRectHeight * 2.f) + 5.f, CP_Color_Create(0, 0, 0, 255), 55.f);

	// PH
	panel = init_panel(800, 400, _windowWidth / 2, _windowHeight / 2);
	slider = init_slider(600, 16, _windowWidth / 2, _windowHeight / 2, 8.f);
}

void main_menu_update(void) {
	CP_Graphics_ClearBackground(bg);

	CP_Settings_TextSize(125.0f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Font_DrawText("CHOCO SURVIVOR", _buttonRectX, _windowHeight / 6.5f);

	float mouseX = CP_Input_GetMouseWorldX(),
		mouseY = CP_Input_GetMouseWorldY();

	render_button(playButton);
	render_button(optionsButton);
	render_button(quitButton);
	update_button_state(&playButton, mouseX, mouseY);
	update_button_state(&optionsButton, mouseX, mouseY);
	update_button_state(&quitButton, mouseX, mouseY);

	render_panel(panel);
	render_slider(slider);

	float cursorWidth = 60.f;
	CP_Image_Draw(cursor, mouseX, mouseY, cursorWidth,
		get_preserved_heightf(CP_Image_GetWidth(cursor), CP_Image_GetHeight(cursor), cursorWidth), 255);

	// Play
	if (playButton.state == BUTTON_STATE_CLICKED)
		CP_Engine_SetNextGameState(level_one_init, level_one_update, level_one_exit);

  	// Options
	if (optionsButton.state == BUTTON_STATE_CLICKED)
		printf("Clicked Options\n");

  	// Maybe Credits?

	// Quit
	if (quitButton.state == BUTTON_STATE_CLICKED)
		CP_Engine_Terminate();
}

void main_menu_exit(void) {
	dispose_sounds();
	CP_Image_Free(&cursor);
	dispose_button(&playButton);
	dispose_button(&optionsButton);
	dispose_button(&quitButton);
	dispose_panel(&panel);
	dispose_slider(&slider);
}
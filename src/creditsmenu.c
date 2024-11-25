#include "cprocessing.h"
#include "mainmenu.h"
#include "button.h"
#include "panel.h"
#include <stdio.h>

static bool _isCreditsActive = false;
static bool _isInitialized;
static struct Button _backToMenuButton;
static struct Rectangle _creditsPanel = { .width = 800, .height = 550 };

static void _init(float window_width, float window_height) {
	CP_Color white = CP_Color_Create(255, 255, 255, 255);
	_creditsPanel.pos.x = window_width / 2.f;
	_creditsPanel.pos.y = window_height / 2.f;

	_backToMenuButton = init_button(_creditsPanel.pos.x, _creditsPanel.pos.y + _creditsPanel.height / 2.5f,
		200.f, 50.f, BUTTON_TYPE_DEFAULT, "Back to Menu", white, 24.f);
}

bool is_credits_menu_open() {
	return _isCreditsActive;
}

void render_credits_menu(float mouse_x, float mouse_y, float window_width, float window_height) {
	if (!_isInitialized) {
		_isInitialized = true;
		_init(window_width, window_height);

		// Open credits
		_isCreditsActive = true;
	}

	// Semi translucent background
	CP_Settings_StrokeWeight(0.f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 185));
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Graphics_DrawRect(0, 0, window_width, window_height);
	CP_Settings_RectMode(CP_POSITION_CENTER);

	render_panel(_creditsPanel.width, _creditsPanel.height, _creditsPanel.pos.x, _creditsPanel.pos.y);
	render_button(_backToMenuButton);

	CP_Settings_TextSize(65.0f);
	CP_Font_DrawText("CREDITS", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 2.f) * 1.55f);

	CP_Settings_TextSize(30.0f);
	
	
	update_button_state(&_backToMenuButton, mouse_x, mouse_y);

	// Save
	if (_backToMenuButton.state == BUTTON_STATE_CLICKED || CP_Input_KeyTriggered(KEY_ESCAPE)) {
		_isInitialized = false;
		_isCreditsActive = false;
	}
	CP_Settings_StrokeWeight(3.f);
}
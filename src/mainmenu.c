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

static struct Button _playButton, _optionsButton, _quitButton;
static CP_Image _cursor;
static CP_Color _bg;

static int _windowWidth, _windowHeight;
static float _buttonRectX, _buttonRectY, _buttonRectHeight, _buttonRectWidth;
static bool _isOptionsActive = false;

// Options Menu
static float _musicVol, _sfxVol, _uiVol;
static struct Button _applyButton, _defaultButton;
static struct Panel _optionsPanel;
static struct Slider _musicVolSlider, _sfxVolSlider, _uiVolSlider;

void main_menu_init(void){
	init_sounds();
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

	CP_Color black = CP_Color_Create(0, 0, 0, 255);
	_playButton = init_button_image(_buttonRectX, _buttonRectY - (_buttonRectHeight * 2.f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_SUCCESS,
		"Play", black, 55.f);

	_optionsButton = init_button_image(_buttonRectX, _buttonRectY, _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_DEFAULT,
		"Options", black, 55.f);

	_quitButton = init_button_image(_buttonRectX, _buttonRectY + (_buttonRectHeight * 2.f), _buttonRectWidth, _buttonRectHeight, BUTTON_TYPE_WARNING,
		"Quit", black, 55.f);

	// Options
	_optionsPanel = init_panel(800, 550, _windowWidth / 2.f, _windowHeight / 2.f);

	float buttonWidth = 125.f;
	_applyButton = init_button_image(_optionsPanel.rect.pos.x + _optionsPanel.rect.width / 2.65f, _optionsPanel.rect.pos.y + _optionsPanel.rect.height / 2.35f,
		buttonWidth, 40.f, BUTTON_TYPE_DEFAULT, "Apply", black, 20.f);
	_defaultButton = init_button_image(_applyButton.rect.pos.x - (buttonWidth * 1.25f), _applyButton.rect.pos.y,
		buttonWidth, 40.f, BUTTON_TYPE_WARNING, "Default", black, 20.f);

	float sliderLength = _optionsPanel.rect.width / 2.5f,
		sliderY = (_optionsPanel.rect.pos.y - _optionsPanel.rect.height / 2.f) * 2.25f;
	_musicVolSlider = init_slider(sliderLength, 16, _optionsPanel.rect.pos.x, sliderY, 26.5f);
	_sfxVolSlider = init_slider(sliderLength, 16, _optionsPanel.rect.pos.x, sliderY * 1.2f, 26.5f);
	_uiVolSlider = init_slider(sliderLength, 16, _optionsPanel.rect.pos.x, sliderY * 1.4f, 26.5f);
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
	if (!_isOptionsActive) {
		update_button_state(&_playButton, mouseX, mouseY);
		update_button_state(&_optionsButton, mouseX, mouseY);
		update_button_state(&_quitButton, mouseX, mouseY);
	}

	// Options menu is open
	if (_isOptionsActive) {
		// Semi translucent background
		CP_Settings_Fill(CP_Color_Create(0, 0, 0, 185));
		CP_Settings_RectMode(CP_POSITION_CORNER);
		CP_Graphics_DrawRect(0, 0, _windowWidth, _windowHeight);
		CP_Settings_RectMode(CP_POSITION_CENTER);

		render_panel(_optionsPanel);
		
		render_slider(_musicVolSlider);
		render_slider(_sfxVolSlider);
		render_slider(_uiVolSlider);

		render_button(_applyButton);
		render_button(_defaultButton);

		CP_Settings_TextSize(65.0f);
		CP_Font_DrawText("SETTINGS", _optionsPanel.rect.pos.x, (_optionsPanel.rect.pos.y - _optionsPanel.rect.height / 2.f) * 1.55f);

		CP_Settings_TextSize(30.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_RIGHT, CP_TEXT_ALIGN_V_MIDDLE);
		// Names
		CP_Font_DrawText("Music", (_musicVolSlider.track.pos.x - _musicVolSlider.track.width / 2.f) / 1.04f,
			_musicVolSlider.track.pos.y);
		CP_Font_DrawText("SFX", (_sfxVolSlider.track.pos.x - _sfxVolSlider.track.width / 2.f) / 1.04f,
			_sfxVolSlider.track.pos.y);
		CP_Font_DrawText("Interface", (_uiVolSlider.track.pos.x - _uiVolSlider.track.width / 2.f) / 1.04f,
			_uiVolSlider.track.pos.y);

		// Values
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
		char musicVal[5], sfxVal[5], uiVal[5];
		sprintf_s(musicVal, sizeof(musicVal), "%.0f%%", _musicVol * 100.f);
		sprintf_s(sfxVal, sizeof(sfxVal), "%.0f%%", _sfxVol * 100.f);
		sprintf_s(uiVal, sizeof(uiVal), "%.0f%%", _uiVol * 100.f);
		CP_Font_DrawText(musicVal, (_musicVolSlider.track.pos.x + _musicVolSlider.track.width / 2.f) * 1.04f,
			_musicVolSlider.track.pos.y);
		CP_Font_DrawText(sfxVal, (_sfxVolSlider.track.pos.x + _sfxVolSlider.track.width / 2.f) * 1.04f,
			_sfxVolSlider.track.pos.y);
		CP_Font_DrawText(uiVal, (_uiVolSlider.track.pos.x + _uiVolSlider.track.width / 2.f) * 1.04f,
			_uiVolSlider.track.pos.y);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

		update_button_state(&_applyButton, mouseX, mouseY);
		update_button_state(&_defaultButton, mouseX, mouseY);
		update_slider_by_mouse(&_uiVolSlider, mouseX, mouseY);
		update_slider_by_mouse(&_musicVolSlider, mouseX, mouseY);
		update_slider_by_mouse(&_sfxVolSlider, mouseX, mouseY);

		// Continuously update values if slider was moved with mouse
		_uiVol = _uiVolSlider.value;
		_musicVol = _musicVolSlider.value;
		_sfxVol = _sfxVolSlider.value;
		set_volume_group(SOUNDGROUP_UI, _uiVol);
		set_volume_group(SOUNDGROUP_MUSIC, _musicVol);
		set_volume_group(SOUNDGROUP_SFX, _sfxVol);

		// Save
		if (_applyButton.state == BUTTON_STATE_CLICKED || CP_Input_KeyTriggered(KEY_ESCAPE)) {
			_isOptionsActive = false;
		}
		// Reset to default
		else if (_defaultButton.state == BUTTON_STATE_CLICKED) {
			// Set to default values
			_musicVol = _uiVol = _sfxVol = 1.f;

			// Update
			update_slider(&_uiVolSlider, _uiVol);
			update_slider(&_musicVolSlider, _musicVol);
			update_slider(&_sfxVolSlider, _sfxVol);
		}
	}
	else {
		// Play
		if (_playButton.state == BUTTON_STATE_CLICKED)
			CP_Engine_SetNextGameState(level_one_init, level_one_update, level_one_exit);

		// Options
		if (_optionsButton.state == BUTTON_STATE_CLICKED) {
			// Get values
			_sfxVolSlider.value = _sfxVol = get_volume_group(SOUNDGROUP_SFX);
			_musicVolSlider.value = _musicVol = get_volume_group(SOUNDGROUP_MUSIC);
			_uiVolSlider.value = _uiVol = get_volume_group(SOUNDGROUP_UI);

			// Open options
			_isOptionsActive = true;

			// Update
			update_slider(&_uiVolSlider, _uiVol);
			update_slider(&_musicVolSlider, _musicVol);
			update_slider(&_sfxVolSlider, _sfxVol);
		}

		// Maybe Credits?

		// Quit
		if (_quitButton.state == BUTTON_STATE_CLICKED)
			CP_Engine_Terminate();
	}

	CP_Settings_ImageMode(CP_POSITION_CORNER);
	float cursorWidth = 60.f;
	// 20.f is to correct its draw position with the actual cursor position
	CP_Image_Draw(_cursor, mouseX - 20.f, mouseY, cursorWidth,
		get_preserved_heightf(CP_Image_GetWidth(_cursor), CP_Image_GetHeight(_cursor), cursorWidth), 255);
	CP_Settings_ImageMode(CP_POSITION_CENTER);
}

void main_menu_exit(void) {
	CP_Image_Free(&_cursor);

	dispose_sounds();

	dispose_button(&_playButton);
	dispose_button(&_optionsButton);
	dispose_button(&_quitButton);
	dispose_button(&_applyButton);
	dispose_button(&_defaultButton);

	dispose_panel(&_optionsPanel);

	dispose_slider(&_musicVolSlider);
	dispose_slider(&_sfxVolSlider);
	dispose_slider(&_uiVolSlider);
}
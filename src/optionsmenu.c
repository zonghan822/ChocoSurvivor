#include "cprocessing.h"
#include "hittest.h"
#include "button.h"
#include "slider.h"
#include "audio.h"
#include "panel.h"
#include <stdbool.h>
#include <stdio.h>

static bool _isOptionsActive = false;
static bool _isInitialized;
static float _musicVol, _sfxVol, _uiVol;
static struct Button _applyButton, _defaultButton;
static struct Slider _musicVolSlider, _sfxVolSlider, _uiVolSlider;
static struct Rectangle _optionsPanel = { .width = 800, .height = 550 };

static void _init(float window_width, float window_height) {
	CP_Color white = CP_Color_Create(255, 255, 255, 255);
	_optionsPanel.pos.x = window_width / 2.f;
	_optionsPanel.pos.y = window_height / 2.f;

	float buttonWidth = 125.f;
	_applyButton = init_button(_optionsPanel.pos.x + _optionsPanel.width / 2.65f, _optionsPanel.pos.y + _optionsPanel.height / 2.35f,
		buttonWidth, 40.f, BUTTON_TYPE_PRIMARY, "Apply", white, 20.f);
	_defaultButton = init_button(_applyButton.rect.pos.x - (buttonWidth * 1.25f), _applyButton.rect.pos.y,
		buttonWidth, 40.f, BUTTON_TYPE_WARNING, "Default", white, 20.f);

	float sliderLength = _optionsPanel.width / 2.5f,
		sliderY = (_optionsPanel.pos.y - _optionsPanel.height / 2.f) * 2.25f;
	_musicVolSlider = init_slider(sliderLength, 16, _optionsPanel.pos.x, sliderY, 26.5f);
	_sfxVolSlider = init_slider(sliderLength, 16, _optionsPanel.pos.x, sliderY * 1.2f, 26.5f);
	_uiVolSlider = init_slider(sliderLength, 16, _optionsPanel.pos.x, sliderY * 1.4f, 26.5f);
}

bool is_options_menu_open() {
	return _isOptionsActive;
}

void render_options_menu(float mouse_x, float mouse_y, float window_width, float window_height) {
	if (!_isInitialized) {
		_isInitialized = true;
		_init(window_width, window_height);

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

	// Semi translucent background
	CP_Settings_StrokeWeight(0.f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 185));
	CP_Settings_RectMode(CP_POSITION_CORNER);
	CP_Graphics_DrawRect(0, 0, window_width, window_height);
	CP_Settings_RectMode(CP_POSITION_CENTER);

	render_panel(_optionsPanel.width, _optionsPanel.height, _optionsPanel.pos.x, _optionsPanel.pos.y);

	render_slider(_musicVolSlider);
	render_slider(_sfxVolSlider);
	render_slider(_uiVolSlider);

	render_button(_applyButton);
	render_button(_defaultButton);

	CP_Settings_TextSize(65.0f);
	CP_Font_DrawText("SETTINGS", _optionsPanel.pos.x, (_optionsPanel.pos.y - _optionsPanel.height / 2.f) * 1.55f);

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

	update_button_state(&_applyButton, mouse_x, mouse_y);
	update_button_state(&_defaultButton, mouse_x, mouse_y);
	update_slider_by_mouse(&_uiVolSlider, mouse_x, mouse_y);
	update_slider_by_mouse(&_musicVolSlider, mouse_x, mouse_y);
	update_slider_by_mouse(&_sfxVolSlider, mouse_x, mouse_y);

	// Continuously update values if slider was moved with mouse
	_uiVol = _uiVolSlider.value;
	_musicVol = _musicVolSlider.value;
	_sfxVol = _sfxVolSlider.value;
	set_volume_group(SOUNDGROUP_UI, _uiVol);
	set_volume_group(SOUNDGROUP_MUSIC, _musicVol);
	set_volume_group(SOUNDGROUP_SFX, _sfxVol);

	// Save
	// (Values are continuously updated, the Apply button is for show)
	if ((!is_mouse_on_rect(_optionsPanel, mouse_x, mouse_y) && CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
		|| _applyButton.state == BUTTON_STATE_CLICKED || CP_Input_KeyTriggered(KEY_ESCAPE)) {
		_isInitialized = false;
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
	CP_Settings_StrokeWeight(3.f);
}
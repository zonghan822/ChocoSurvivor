#include "pausemenu.h"
#include "level1.h"

static struct Button resumeButton, restartButton, quitButton;

void pause_menu_init() {
    // Load overlay image for the pause menu
    _pmenu.overlayImage = CP_Image_Load("Assets/GUI/tilemap_large.png");

    // Initialize buttons
    _pmenu.resumeButton = init_button(400, 200, 150, 50, BUTTON_TYPE_SUCCESS, CP_Color_Create(0, 255, 0, 255), "Resume", CP_Color_Create(255, 255, 255, 255), 24);
    _pmenu.restartButton = init_button(400, 300, 150, 50, BUTTON_TYPE_WARNING, CP_Color_Create(255, 165, 0, 255), "Restart", CP_Color_Create(255, 255, 255, 255), 24);
    _pmenu.quitButton = init_button(400, 400, 150, 50, BUTTON_TYPE_DEFAULT, CP_Color_Create(255, 0, 0, 255), "Quit", CP_Color_Create(255, 255, 255, 255), 24);
}

void pause_menu(struct ChocoRoach* roach, float width, float height)
{


	int _bg = CP_Color_Create(128, 128, 128, 255);

	float _buttonRectX = width / 2.0f;
	float _buttonRectY = height / 2.0f;
	_buttonRectHeight = 85.0f;
	_buttonRectWidth = 300.0f;

}


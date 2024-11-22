#pragma once
#include "button.h"
#include "cprocessing.h"


struct PauseMenu {
    CP_Image overlayImage;  // Image for the overlay panel
    struct Button resumeButton;  // Resume button
    struct Button restartButton; // Restart button
    struct Button quitButton;    // Quit button
};


extern struct PauseMenu _pmenu;


void pause_menu(struct ChocoRoach* roach, float width, float height);
void pause_menu_init();
void pause_menu_dispose();
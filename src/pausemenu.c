#include "pausemenu.h"
#include "button.h"
#include "hittest.h"
#include "gui.h"
#include "cprocessing.h"

// Pause menu state
static bool isPaused = false;
// Buttons for the pause menu
static struct Button resumeButton;
static struct Button endGameButton;
static struct Button optionsButton;
void init_pause_menu(float screenWidth, float screenHeight) {
    // Initialize tilemap for GUI visuals
    init_tilemap();
    // Initialize buttons
    resumeButton = init_button(screenWidth / 2, screenHeight / 2 - 75, 200, 50, BUTTON_TYPE_SUCCESS, "Resume", CP_Color_Create(255, 255, 255, 255), 24.0f);
    endGameButton = init_button(screenWidth / 2, screenHeight / 2, 200, 50, BUTTON_TYPE_WARNING, "End Game", CP_Color_Create(255, 255, 255, 255), 24.0f);
    optionsButton = init_button(screenWidth / 2, screenHeight / 2 + 75, 200, 50, BUTTON_TYPE_DEFAULT, "Options", CP_Color_Create(255, 255, 255, 255), 24.0f);
}
void render_pause_menu() {
    if (isPaused) {
        // Render a dim background overlay
        CP_Settings_Fill(CP_Color_Create(0, 0, 0, 128));
        CP_Graphics_DrawRect(0, 0, CP_System_GetWindowWidth(), CP_System_GetWindowHeight());
        // Render buttons
        render_button(resumeButton);
        render_button(endGameButton);
        render_button(optionsButton);
    }
}
void update_pause_menu(float mouseX, float mouseY) {
    if (isPaused) {
        // Update button states based on mouse interaction
        update_button_state(&resumeButton, mouseX, mouseY);
        update_button_state(&endGameButton, mouseX, mouseY);
        update_button_state(&optionsButton, mouseX, mouseY);
        // Check for button clicks and handle actions
        if (resumeButton.state == BUTTON_STATE_CLICKED) {
            isPaused = false; // Resume the game
        }
        if (endGameButton.state == BUTTON_STATE_CLICKED) {
            // Exit the game (or go back to the main menu)
            CP_Engine_Terminate();
        }
        if (optionsButton.state == BUTTON_STATE_CLICKED) {
            // Trigger options menu logic (implement as needed)
            open_options_menu();
        }
    }
}
void toggle_pause_menu() {
    isPaused = !isPaused;

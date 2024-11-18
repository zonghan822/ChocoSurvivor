#include "mainmenu.h"
#include "level1.h"
#include "level2.h"
#include "hittest.h"
#include "cprocessing.h"
#include "audio.h"

#include <stdio.h>

struct Rectangle playRect, optionsRect, quitRect;
CP_Image cursor;
CP_Color bg, playBg, otherBg;

void main_menu_init(void){
	init_sounds();
	cursor = CP_Image_Load("Assets/cursor.png");
	CP_System_ShowCursor(FALSE);

	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

	bg = CP_Color_Create(128, 128, 128, 255);
	playBg = CP_Color_Create(144, 238, 144, 255);
	otherBg = CP_Color_Create(169, 169, 169, 255);
}

void main_menu_update(void) {
	CP_Graphics_ClearBackground(bg);

	int windowWidth = CP_System_GetWindowWidth(),
		windowHeight = CP_System_GetWindowHeight();

	float buttonRectX = windowWidth / 2.0f,
		buttonRectY = windowHeight / 2.0f,
		buttonRectHeight = 85.0f,
		buttonRectWidth = 300.0f;

	// Play button
	CP_Settings_Fill(playBg);
	playRect.pos.x = buttonRectX;
	playRect.pos.y = buttonRectY - (buttonRectHeight * 2.0f);
	playRect.width = buttonRectWidth;
	playRect.height = buttonRectHeight;
	CP_Graphics_DrawRect(playRect.pos.x, playRect.pos.y, playRect.width, playRect.height);

	// Options button
	CP_Settings_Fill(otherBg);
	optionsRect.pos.x = buttonRectX;
	optionsRect.pos.y = buttonRectY;
	optionsRect.width = buttonRectWidth;
	optionsRect.height = buttonRectHeight;
	CP_Graphics_DrawRect(optionsRect.pos.x, optionsRect.pos.y, optionsRect.width, optionsRect.height);

	// Quit button
	quitRect.pos.x = buttonRectX;
	quitRect.pos.y = buttonRectY + (buttonRectHeight * 2.0f);
	quitRect.width = buttonRectWidth;
	quitRect.height = buttonRectHeight;
	CP_Graphics_DrawRect(quitRect.pos.x, quitRect.pos.y, quitRect.width, quitRect.height);

	CP_Settings_TextSize(125.0f);
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Font_DrawText("CHOCO SURVIVOR", buttonRectX, windowHeight / 6.5f);
	CP_Settings_TextSize(55.0f);
	CP_Font_DrawText("Play", playRect.pos.x, playRect.pos.y + 5.0f);
	CP_Font_DrawText("Options", optionsRect.pos.x, optionsRect.pos.y + 5.0f);
	CP_Font_DrawText("Quit", quitRect.pos.x, quitRect.pos.y + 5.0f);

	float mouse_x = CP_Input_GetMouseWorldX(),
		mouse_y = CP_Input_GetMouseWorldY();
	CP_Image_Draw(cursor, mouse_x, mouse_y, 70, 50, 255);
	// Play
	if (is_rect_clicked(playRect, mouse_x, mouse_y)) {
		play_sound(UI_BUTTON_HOVER);

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
			play_sound(UI_BUTTON_CONFIRM_CLICK);
			CP_Engine_SetNextGameState(level_one_init, level_one_update, level_one_exit);
		}
	}

  	// Options
	else if (is_rect_clicked(optionsRect, mouse_x, mouse_y)) {
		play_sound(UI_BUTTON_HOVER);

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
			play_sound(UI_BUTTON_CONFIRM_CLICK);
			printf("Clicked Options\n"); // Set option state here
		}
	}

  	// Maybe Credits?

	// Quit
	else if (is_rect_clicked(quitRect, mouse_x, mouse_y)) {
		play_sound(UI_BUTTON_HOVER);

		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
			play_sound(UI_BUTTON_CONFIRM_CLICK);
			CP_Engine_Terminate();
		}
	}
}

void main_menu_exit(void) {
	dispose_sounds();
	CP_Image_Free(&cursor);
}
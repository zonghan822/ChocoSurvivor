#include "mainmenu.h"
#include "game.h"
#include "hittest.h"
#include "cprocessing.h"
#include <stdio.h>

struct Rectangle gPlayRect, gOptionsRect, gQuitRect;

void main_menu_init(void)
{
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_TextSize(55.0f);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

	CP_Color bg = CP_Color_Create(128, 128, 128, 255);
	CP_Color playBg = CP_Color_Create(144, 238, 144, 255);
	CP_Color otherBg = CP_Color_Create(169, 169, 169, 255);
	CP_Graphics_ClearBackground(bg);

	int windowWidth = CP_System_GetWindowWidth(),
		windowHeight = CP_System_GetWindowHeight();

	float buttonRectX = windowWidth / 2.0f,
		buttonRectY = windowHeight / 2.0f,
		buttonRectHeight = 85.0f,
		buttonRectWidth = 300.0f;

	// Play button
	CP_Settings_Fill(playBg);
	gPlayRect.pos.x = buttonRectX;
	gPlayRect.pos.y = buttonRectY - (buttonRectHeight * 2.0f);
	gPlayRect.width = buttonRectWidth;
	gPlayRect.height = buttonRectHeight;
	CP_Graphics_DrawRect(gPlayRect.pos.x, gPlayRect.pos.y, gPlayRect.width, gPlayRect.height);

	// Options button
	CP_Settings_Fill(otherBg);
	gOptionsRect.pos.x = buttonRectX;
	gOptionsRect.pos.y = buttonRectY;
	gOptionsRect.width = buttonRectWidth;
	gOptionsRect.height = buttonRectHeight;
	CP_Graphics_DrawRect(gOptionsRect.pos.x, gOptionsRect.pos.y, gOptionsRect.width, gOptionsRect.height);

	// Quit button
	gQuitRect.pos.x = buttonRectX;
	gQuitRect.pos.y = buttonRectY + (buttonRectHeight * 2.0f);
	gQuitRect.width = buttonRectWidth;
	gQuitRect.height = buttonRectHeight;
	CP_Graphics_DrawRect(gQuitRect.pos.x, gQuitRect.pos.y, gQuitRect.width, gQuitRect.height);

	// Text
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Font_DrawText("Play", gPlayRect.pos.x, gPlayRect.pos.y + 5.0f);
	CP_Font_DrawText("Options", gOptionsRect.pos.x, gOptionsRect.pos.y + 5.0f);
	CP_Font_DrawText("Quit", gQuitRect.pos.x, gQuitRect.pos.y + 5.0f);
}

void main_menu_update(void)
{
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
	{
		float mouse_x = CP_Input_GetMouseWorldX(),
			mouse_y = CP_Input_GetMouseWorldY();

		// Play
		if (is_rect_clicked(gPlayRect, mouse_x, mouse_y)) 
			//printf("Clicked Play\n"); // Set game state here
			CP_Engine_SetNextGameState(game_init, game_update, game_exit);

		// Options
		else if (is_rect_clicked(gOptionsRect, mouse_x, mouse_y))
			printf("Clicked Options\n"); // Set option state here

		// Maybe Credits?

		// Quit
		else if (is_rect_clicked(gQuitRect, mouse_x, mouse_y))
			CP_Engine_Terminate();
	}
}

void main_menu_exit(void)
{

}
#include "mainmenu.h"
#include "game.h"
#include "hittest.h"
#include "cprocessing.h"
#include <stdio.h>

const float BUTTON_RECT_W = 300.0f,
	BUTTON_RECT_H = 85.0f,
	BUTTON_FONT_SIZE = 55.0f;

float g_play_rect_x, g_play_rect_y, g_options_rect_x, g_options_rect_y, g_quit_rect_x, g_quit_rect_y;
CP_Image chococursor;


void main_menu_init(void)
{
	
	chococursor = CP_Image_Load("Assets/cursor.png");
	CP_System_ShowCursor(FALSE);
		
}

void main_menu_update(void)
{
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Settings_RectMode(CP_POSITION_CENTER);
	CP_Settings_TextSize(BUTTON_FONT_SIZE);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/Oxanium/Oxanium-Variable.ttf"));

	int window_w = CP_System_GetWindowWidth(),
		window_h = CP_System_GetWindowHeight();

	float button_rect_x = window_w / 2.0f,
		button_rect_y = window_h / 2.0f;

	CP_Color bg = CP_Color_Create(128, 128, 128, 255);
	CP_Color play_bg = CP_Color_Create(144, 238, 144, 255);
	CP_Color other_bg = CP_Color_Create(169, 169, 169, 255);
	CP_Graphics_ClearBackground(bg);
	
	// Play button
	CP_Settings_Fill(play_bg);
	g_play_rect_x = button_rect_x;
	g_play_rect_y = button_rect_y - (BUTTON_RECT_H * 2.0f);
	CP_Graphics_DrawRect(g_play_rect_x, g_play_rect_y, BUTTON_RECT_W, BUTTON_RECT_H);

	// Options button
	CP_Settings_Fill(other_bg);
	g_options_rect_x = button_rect_x;
	g_options_rect_y = button_rect_y;
	CP_Graphics_DrawRect(g_options_rect_x, g_options_rect_y, BUTTON_RECT_W, BUTTON_RECT_H);

	// Quit button
	g_quit_rect_x = button_rect_x;
	g_quit_rect_y = button_rect_y + (BUTTON_RECT_H * 2.0f);
	CP_Graphics_DrawRect(g_quit_rect_x, g_quit_rect_y, BUTTON_RECT_W, BUTTON_RECT_H);

	// Text
	CP_Settings_Fill(CP_Color_Create(0, 0, 0, 255));
	CP_Font_DrawText("Play", g_play_rect_x, g_play_rect_y + 5.0f);
	CP_Font_DrawText("Options", g_options_rect_x, g_options_rect_y + 5.0f);
	CP_Font_DrawText("Quit", g_quit_rect_x, g_quit_rect_y + 5.0f);

	

	float mouse_x = CP_Input_GetMouseWorldX();
	float mouse_y = CP_Input_GetMouseWorldY();
	CP_Image_Draw(chococursor, mouse_x, mouse_y, 70, 50, 255);
	if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
	{
		
		
		// Play
		if (IsRectClicked(g_play_rect_x, g_play_rect_y, BUTTON_RECT_W, BUTTON_RECT_H, mouse_x, mouse_y, CP_POSITION_CENTER)) 
			//printf("Clicked Play\n"); // Set game state here
			CP_Engine_SetNextGameState(game_init, game_update, game_exit);

		// Options
		else if (IsRectClicked(g_options_rect_x, g_options_rect_y, BUTTON_RECT_W, BUTTON_RECT_H, mouse_x, mouse_y, 0))
			printf("Clicked Options\n"); // Set option state here

		// Maybe Credits?

		// Quit
		else if (IsRectClicked(g_quit_rect_x, g_quit_rect_y, BUTTON_RECT_W, BUTTON_RECT_H, mouse_x, mouse_y, 0))
			CP_Engine_Terminate();
	}
}

void main_menu_exit(void)
{

}
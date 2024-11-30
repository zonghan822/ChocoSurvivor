/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
*@filename		creditsmenu.c
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This file includes function to render the credits menu and a boolean function to
return the status of the menu. 
Referred to the optionsmenu.c developed by Muhammad Nur Fadzly when creating this 
file.
******************************************************************************/

#include "cprocessing.h"
#include "mainmenu.h"
#include "button.h"
#include "panel.h"
#include <stdio.h>

static bool _isCreditsActive = false;
static bool _isInitialized;
static struct Button _backToMenuButton;
static struct Rectangle _creditsPanel = { .width = 950.f, .height = 700.f };
static CP_Image _tutImageLmb;

// state of current page
static enum CreditsPage {
	PAGE_NULL,
	CREDITS_PAGE_ONE,
	CREDITS_PAGE_TWO,
	CREDITS_PAGE_THREE
} creditsPage;


static void _init(float window_width, float window_height) {
	CP_Color white = CP_Color_Create(255, 255, 255, 255);
	_creditsPanel.pos.x = window_width / 2.f;
	_creditsPanel.pos.y = window_height / 2.f;

	_backToMenuButton = init_button(_creditsPanel.pos.x, _creditsPanel.pos.y + _creditsPanel.height / 2.5f,
		200.f, 50.f, BUTTON_TYPE_PRIMARY, "Back to Menu", white, 24.f);

	creditsPage = PAGE_NULL;

	_tutImageLmb = CP_Image_Load("Assets/GUI/mouse_left.png");
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

	CP_Settings_TextSize(25.0f);
	CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
	CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/PixelifySans/PixelifySans-VariableFont_wght.ttf"));
	CP_Font_DrawText("All content \xC2\xA9 2024 DigiPen Institute of Technology Singapore.", _creditsPanel.pos.x, (_creditsPanel.pos.y + _creditsPanel.height / 3.f) - 20.f);
	CP_Font_DrawText("All Rights Reserved", _creditsPanel.pos.x, _creditsPanel.pos.y + _creditsPanel.height / 3.f);
	CP_Font_Set(CP_Font_Load("Assets/Fonts/Tiny5/Tiny5-Regular.ttf"));

	switch (creditsPage) {
	case CREDITS_PAGE_ONE: {
		CP_Settings_TextSize(65.0f);
		CP_Font_DrawText("CREDITS", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 2.f) * 1.65f);
		CP_Settings_Fill(CP_Color_Create(128, 0, 0, 255));
		CP_Settings_TextSize(45.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("TEAM CHOCO LATTE", (_creditsPanel.pos.x - _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f));
		CP_Font_DrawText("INSTRUCTORS", (_creditsPanel.pos.x + _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f));
		CP_Font_DrawText("SPECIAL THANKS", _creditsPanel.pos.x, _creditsPanel.pos.y + 80.f);

		float lineGap = 50.f;
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_TextSize(40.0f);
		CP_Font_DrawText("LAW ZONG HAN", (_creditsPanel.pos.x - _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f) + lineGap);
		CP_Font_DrawText("DALTON KOH", (_creditsPanel.pos.x - _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f + (2 * lineGap)));
		CP_Settings_TextSize(30.0f);
		CP_Font_DrawText("MUHAMMAD NUR FADZLY", (_creditsPanel.pos.x - _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f + (3 * lineGap)));
		CP_Font_DrawText("BIN ZULKIFLI", (_creditsPanel.pos.x - _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f + (3 * lineGap) + 30.f));

		CP_Settings_TextSize(40.0f);
		CP_Font_DrawText("CHENG DING XIANG", (_creditsPanel.pos.x + _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f) + lineGap);
		CP_Font_DrawText("GERALD WONG", (_creditsPanel.pos.x + _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f + (2 * lineGap)));
		CP_Settings_TextSize(30.0f);
		CP_Font_DrawText("SOROOR MALEKMOHMMADI", (_creditsPanel.pos.x + _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f + (3 * lineGap)));
		CP_Font_DrawText("FARADOUNBEH", (_creditsPanel.pos.x + _creditsPanel.width / 4.f), (_creditsPanel.pos.y - _creditsPanel.height / 4.f + (3 * lineGap) + 30.f));

		CP_Settings_TextSize(40.0f);
		CP_Font_DrawText("ANJANA RAJENDRAN", _creditsPanel.pos.x, _creditsPanel.pos.y + 80.f + lineGap);

		break;
	}
	case CREDITS_PAGE_TWO: {
		CP_Settings_TextSize(45.0f);
		CP_Settings_Fill(CP_Color_Create(128, 0, 0, 255));
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("CREATED AT", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 2) + 80.f);
		CP_Font_DrawText("PRESIDENT", _creditsPanel.pos.x, _creditsPanel.pos.y - 140.f);
		CP_Font_DrawText("EXECUTIVES", _creditsPanel.pos.x, _creditsPanel.pos.y);

		float lineGap = 50.f;
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_TextSize(40.0f);
		CP_Font_DrawText("DigiPen Institute of Technology Singapore", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 2) + 80.f + lineGap);
		CP_Font_DrawText("CLAUDE COMAIR", _creditsPanel.pos.x, _creditsPanel.pos.y - 150.f + lineGap);

		lineGap = 40.f;
		CP_Settings_TextSize(35.0f);
		CP_Font_DrawText("JASON CHU   MICHELE COMAIR   CHRISTOPHER COMAIR", _creditsPanel.pos.x, _creditsPanel.pos.y + lineGap);
		CP_Font_DrawText("ALEXANDER COMAIR   MICHAEL GATS   TAN CHEK MING", _creditsPanel.pos.x, _creditsPanel.pos.y + (2 * lineGap));
		CP_Font_DrawText("PRASANNA GHALI   MANDY WONG   JOHNNY DEEK", _creditsPanel.pos.x, _creditsPanel.pos.y + (3 * lineGap));
		break;
	}
	case CREDITS_PAGE_THREE: {
		CP_Settings_TextSize(45.0f);
		CP_Settings_Fill(CP_Color_Create(128, 0, 0, 255));
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("ASSETS USED", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 3));

		float lineGap = 50.f;
		CP_Settings_Fill(CP_Color_Create(255, 255, 255, 255));
		CP_Settings_TextSize(40.0f);
		CP_Font_DrawText("Music Jingles BY Kenney.nl", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 3) + lineGap);
		CP_Font_DrawText("Interface Sounds BY Kenney.nl", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 3) + (2 * lineGap));
		CP_Font_DrawText("UI Pack - Pixel Adventure BY Kenney.nl", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 3) + (3 * lineGap));
		CP_Font_DrawText("Not Jam Music Pack BY Not Jam", _creditsPanel.pos.x, (_creditsPanel.pos.y - _creditsPanel.height / 3) + (4 * lineGap));
	}
	}
	
	
	update_button_state(&_backToMenuButton, mouse_x, mouse_y);

	if (is_mouse_on_rect(_creditsPanel, mouse_x, mouse_y)) {
		if (CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT)) {
			creditsPage = creditsPage + 1 > 3 ? 1 : creditsPage + 1;
		}

		float lmbImgW = 50.f, lmbImgH = 50.f;
		CP_Image_Draw(_tutImageLmb, mouse_x + lmbImgW, mouse_y, lmbImgW, lmbImgH, 255);
		CP_Settings_TextSize(20.0f);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_LEFT, CP_TEXT_ALIGN_V_MIDDLE);
		CP_Font_DrawText("Next Page", mouse_x + lmbImgW * 1.5f, mouse_y);
		CP_Settings_TextAlignment(CP_TEXT_ALIGN_H_CENTER, CP_TEXT_ALIGN_V_MIDDLE);

	}

	// Save
	if ((!is_mouse_on_rect(_creditsPanel, mouse_x, mouse_y) && CP_Input_MouseTriggered(MOUSE_BUTTON_LEFT))
		|| _backToMenuButton.state == BUTTON_STATE_CLICKED || CP_Input_KeyTriggered(KEY_ESCAPE)) {
		_isInitialized = false;
		_isCreditsActive = false;
		CP_Image_Free(&_tutImageLmb);
	}
	CP_Settings_StrokeWeight(3.f);
}
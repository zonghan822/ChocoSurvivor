/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
* @filename		optionsmenu.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file declares the functions for handling options menu in
				its entirety. Also has a function to return a boolean, to
				indicate whether the options menu is active.
******************************************************************************/

#include <stdbool.h>

bool is_options_menu_open(void);

void render_options_menu(float mouse_x, float mouse_y, float window_width, float window_height);
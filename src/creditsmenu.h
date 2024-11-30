/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
*@filename		creditsmenu.h
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This header file declares functions to handle credits menu including checking
for status and rendering of the credits menu. 
Referred to the optionsmenu.c developed by Muhammad Nur Fadzly when creating 
the functions in this file.
******************************************************************************/

#pragma once
#include <stdbool.h>

bool is_credits_menu_open(void);

void render_credits_menu(float mouse_x, float mouse_y, float window_width, float window_height);
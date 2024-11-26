/******************************************************************************
*@filename		creditsmenu.h
*@author(s)		Law Zong Han
*@DP emails		zonghan.law@digipen.edu
*@course		Software Engineering Project 1
*@teamname		Choco Latte
*@gamename		Choco Survivor
*@brief:
This file is a header file for astar pathfinding algorithm for enemy and boss
movement.
******************************************************************************/

#pragma once
#include <stdbool.h>

bool is_credits_menu_open(void);

void render_credits_menu(float mouse_x, float mouse_y, float window_width, float window_height);
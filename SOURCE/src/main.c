/******************************************************************************
* All content © 2024 DigiPen Institute of Technology Singapore,
* all rights reserved.
*
* @filename		main.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli, Law Zong Han, Dalton Koh
* @DP emails	muhammadnurfadzly.b@digipen.edu, zonghan.law@digipen.edu
*				d.koh@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       This file is the driver file for the game.
******************************************************************************/

#include "cprocessing.h"
#include "mainmenu.h"

// Fixed 1600x900 resolution
int main(void)
{
	CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	CP_System_SetWindowSize(1600, 900);
	CP_Engine_Run(0);
	return 0;
}
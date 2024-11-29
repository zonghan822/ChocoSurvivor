/******************************************************************************
* @filename		gui.h
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli,
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       Fairly simple file to just init tilemaps and dispose tilemaps
				for use in level files.
******************************************************************************/

#pragma once
#include "cprocessing.h"

extern CP_Image tilemap;
extern CP_Image tilemapLarge;

void init_tilemap(void);
void dispose_tilemap(void);
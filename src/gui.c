/******************************************************************************
* @filename		gui.c
* @author(s)	Muhammad Nur Fadzly Bin Zulkifli,
* @DP emails	muhammadnurfadzly.b@digipen.edu
* @course		Software Engineering Project 1
* @teamname		Choco Latte
* @gamename		Choco Survivor
* @brief:       Fairly simple file to just init tilemaps and dispose tilemaps
				for use in level files.
******************************************************************************/

#include "gui.h"
#include "cprocessing.h"

CP_Image tilemap;
CP_Image tilemapLarge;

void init_tilemap(void) {
	tilemap = CP_Image_Load("Assets/GUI/tilemap.png");
	tilemapLarge = CP_Image_Load("Assets/GUI/tilemap_large.png");
}

void dispose_tilemap(void) {
	CP_Image_Free(&tilemap);
	CP_Image_Free(&tilemapLarge);
}
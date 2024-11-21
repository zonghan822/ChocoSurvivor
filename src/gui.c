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
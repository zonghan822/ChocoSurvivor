#include "cprocessing.h"
#include "mainmenu.h"

// Do we need to change window size? 
// Keep 1600x900 for now, will change to fullscreen afterwards
int main(void)
{
	CP_System_SetWindowTitle("Choco Survivor");
	CP_Engine_SetNextGameState(main_menu_init, main_menu_update, main_menu_exit);
	CP_System_SetWindowSize(1600, 900);
#if _DEBUG
	CP_Engine_Run(1);
#else
	CP_Engine_Run(0);
#endif
	return 0;
}
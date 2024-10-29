#include "cprocessing.h"
#include "game.h"

// Do we need to change window size?
int main(void)
{
	CP_Engine_SetNextGameState(Game_Init, Game_Update, Game_Exit);
	CP_System_SetWindowSize(1600, 900);
#if _DEBUG
	CP_Engine_Run(1);
#else
	CP_Engine_Run(0);
#endif
	return 0;
}
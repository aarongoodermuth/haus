#include <time.h>

#include "Game.h"


int main()
{
#ifndef _DEBUG
	srand((unsigned int)time(nullptr));
#endif

	Game g = Game();
	g.Play();
	getchar();
	return 0;
}
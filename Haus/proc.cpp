#include <time.h>

#include "Game.h"


int main()
{
#ifndef _DEBUG
	srand((unsigned int)time(NULL));
#endif

	Game g = Game();
	g.Play();
	getchar();
	return 0;
}
#include <time.h>

#include "Game.h"
#include "Player.h"


int main()
{
#ifndef _DEBUG
	srand((unsigned int)time(nullptr));
#endif

	Game g = Game({ std::make_unique<User>() });
	g.Play();
	getchar();
	return 0;
}
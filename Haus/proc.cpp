#include <time.h>

#include "Game.h"
#include "Player.h"


int main()
{
#ifndef _DEBUG
	srand((unsigned int)time(nullptr));
#endif

	std::vector<std::unique_ptr<Player>> vecpopl;
	vecpopl.push_back(std::make_unique<User>());
	Game(std::move(vecpopl)).Play();
	getchar();
	return 0;
}
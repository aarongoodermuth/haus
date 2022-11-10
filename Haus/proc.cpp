#include <time.h>

#include "Game.h"
#include "Player.h"


int main()
{
#ifndef _DEBUG
	srand((unsigned int)time(nullptr));
#endif

	GameView gmvw;
	std::vector<std::unique_ptr<Player>> vecpopl;
	vecpopl.push_back(std::make_unique<User>(&gmvw));
	Game(std::move(vecpopl), &gmvw).Play();
	getchar();
	return 0;
}
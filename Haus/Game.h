#pragma once
#include <vector>

#include "Cards.h"
#include "GameView.h"
#include "Player.h"
#include "Round.h"

class Game
{
public:
	Game();

	void Play();

private:
	vector<std::unique_ptr<Player>> _vecpopl;
	int _iposplDealer;
	GameView _gmvw;


	void DoRound();
	bool FWinnerExists();
	void DetermineWinner();
	void DisplayWinner();
};
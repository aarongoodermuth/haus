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
	~Game();

	void Play();

private:
	vector<Player*> _vecppl;
	int _iposplDealer;
	GameView _gmvw;

	void DoRound();
	bool FWinnerExists();
	void DetermineWinner();
	void DisplayWinner();
};
#pragma once
#include <vector>

#include "Cards.h"
#include "Player.h"
#include "Round.h"

class Game
{
public:
	Game();
	~Game();

	void Play();

	// TODO : Make these both part of a rules class
	static const int cpl = 4;
	static const int ccrdsPerPl = 6;

private:
	vector<Player*> _vecppl;
	int _iposplDealer;

	void DoRound();
	bool FWinnerExists();
	void DetermineWinner();
	void DisplayWinner();
};
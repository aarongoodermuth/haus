#pragma once

#include "Cards.h"
#include "Player.h"

class Game
{
public:
	Game();
	~Game();

	void Play();

private:
	static const int cpl = 4;
	static const int ccrdsPerPl = 6;

	Player *pl[cpl];
	Deck _dck;
	int posplDealer;

	void DealCards();
	void DoBet();
	void DoRound();
	void PlayTricks();
	void DoScoring();
	bool FWinnerExists();
	void DetermineWinner();
	void DisplayWinner();
};
#pragma once

#include "Player.h"

class Game
{
public:
	Game();
	~Game();

	void Play();

private:
	static const int cpl = 4;

	Player *pl[cpl];
	int posplDealer;

	bool FWinnerExists();
	void DisplayWinner();
	void DoBet();
	void DoRound();
	void PlayTricks();
	void DoScoring();
	void DetermineWinner();
};
#pragma once

#include <vector>
#include "Player.h"
#include "Cards.h"

using namespace std;

class Round
{
public:
	Round(const vector<Player*>& vecppl, int iposplDealer);
	void Do();

private:
	vector<Player*> _vecppl;
	Deck _dck;
	int _iposplDealer;

	void DealCards();
	void DoBet();
	void PlayTricks();
	void DoScoring();
};


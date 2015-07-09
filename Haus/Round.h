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
	vector<Card> _veccrdPlayed;
	vector<Card> _veccrdPlayedThisTurn;
	Deck _dck;
	int _iposplDealer;
	int _iposplStartTrick;

	void DealCards();
	void DoBet();
	void PlayTricks();
	void DoScoring();
};


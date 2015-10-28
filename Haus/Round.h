#pragma once

#include <vector>
#include "Cards.h"
#include "Player.h"
#include "RoundView.h"

using namespace std;

class Round
{
public:
	Round(const vector<Player*>& vecppl, int iposplDealer);
	~Round();
	void Do();

private:
	vector<Player*> _vecppl;
	vector<Card> _veccrdPlayed;
	vector<Card> _veccrdPlayedThisTurn;
	Deck _dck;
	int _iposplDealer;
	int _iposplStartTrick;
	RoundView* _prndvw;

	void DealCards();
	void DoBet();
	void PlayTricks();
	void DoScoring();
};


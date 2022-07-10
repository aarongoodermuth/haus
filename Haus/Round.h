#pragma once

#include <memory>
#include <vector>
#include "Cards.h"
#include "Player.h"
#include "RoundView.h"

using namespace std;

class Round
{
public:
	Round(vector<std::unique_ptr<Player>>& vecpopl, int iposplDealer);
	void Do();
	RoundView* PrndvwGet();

private:
	vector<std::unique_ptr<Player>> &_vecpopl;
	vector<Card> _veccrdPlayed;
	vector<Card> _veccrdPlayedThisTurn;
	Deck _dck;
	int _iposplDealer;
	int _iposplStartTrick;
	RoundView _rndvw;
	Suit _suitTrump;

	void DealCards();
	void DoBet();
	void ChooseTrump();
	void PlayTricks();
	void ShowRoundSummary();
	void DoScoring();
	void ResetRound();
};


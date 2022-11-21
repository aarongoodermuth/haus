#pragma once

#include <memory>
#include <vector>
#include "Cards.h"
#include "Player.h"
#include "RoundView.h"


class Round
{
public:
	Round(std::vector<std::unique_ptr<Player>>& vecpopl, int iposplDealer);
	void Do();
	RoundView* PrndvwGet();

private:
	std::vector<std::unique_ptr<Player>> &_vecpopl;
	std::vector<Card> _veccrdPlayed;
	std::vector<Card> _veccrdPlayedThisTurn;
	Deck _dck;
	int _iposplDealer;
	int _iposplBid;
	RoundView _rndvw;
	std::optional<Suit> _optsuitTrump;

	void DealCards();
	void DoBet();
	void ChooseTrump();
	void PlayTricks();
	void ShowRoundSummary();
	void DoScoring();
	void ResetRound();
};


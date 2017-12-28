#include "Round.h"

#include <vector>
#include <iostream>
#include "Cards.h"
#include "Debug.h"
#include "Game.h"
#include "Player.h"
#include "Rules.h"

using namespace std;

Round::Round(const vector<Player*>& vecppl, int iposplDealer) : _rndvw(&_veccrdPlayed, &_veccrdPlayedThisTurn, &_suitTrump)
{
	_vecppl = vecppl;
	_iposplDealer = iposplDealer;
	_suitTrump = Suit::suitNil;
}


void Round::Do()
{
	ResetRound();
	_dck.Reset();
	_dck.Shuffle();
	DealCards();
	DoBet();
	PlayTricks();
	DoScoring();
	ShowRoundSummary();
}

void Round::ResetRound()
{
	_dck.Reset();
	for (Player *ppl : _vecppl)
		ppl->ResetRound();
}

RoundView* Round::PrndvwGet()
{
	return &_rndvw;
}


void Round::DealCards()
{
	for (int i = 0; i < Rules::ccrdsPerPl * Rules::cpl; i++)
	{
		int ipos = (i + _iposplDealer + 1) % Rules::cpl;
		ASSERT(ipos >= 0 && ipos < Rules::cpl);
		ASSERTIMPLIES(i == (Rules::cpl - 1), ipos == _iposplDealer);
		_vecppl[ipos]->TakeCard(_dck.Deal());
	}
}


void Round::DoBet()
{
	BetChoice bcCurHighestBid = BetChoice::DEFAULT_DEALER;
	int iposCurHighBidder = _iposplDealer;
	BetChoice bcCurBet;
	int ipos;
	bool fForcedToTake = true;

	for (int i = 0; i < Rules::cpl; i++)
	{
		ipos = (i + _iposplDealer) % Rules::cpl;
		ASSERT(ipos < Rules::cpl && ipos >= 0);
		bcCurBet = _vecppl[ipos]->BcRequestDesiredBet();
		if (bcCurBet > bcCurHighestBid)
		{
			bcCurHighestBid = bcCurBet;
			iposCurHighBidder = ipos;
			fForcedToTake = false;
		}
	}

	// now we know who will have to bet
	_vecppl[iposCurHighBidder]->ForceBet(bcCurHighestBid, fForcedToTake);
	_iposplStartTrick = iposCurHighBidder;
}


void Round::PlayTricks()
{
	_suitTrump = _vecppl[_iposplDealer]->SuitChooseTrump();

	for (int itrk = 0; itrk < Rules::ccrdsPerPl; itrk++)
	{
		int iposplHighestCard = _iposplStartTrick;
		Card crdLead = _vecppl[_iposplStartTrick]->CrdPlay(NULL);
		Card crdHighest = crdLead;
		for (int i = 1; i < Rules::cpl; i++)
		{
			int ipospl = (_iposplStartTrick + i) % Rules::cpl;
			ASSERT(ipospl >= 0 && ipospl < Rules::cpl);
			Card crdPlayed = _vecppl[ipospl]->CrdPlay(&crdLead);
			_veccrdPlayedThisTurn.push_back(crdPlayed);
			_veccrdPlayed.push_back(crdPlayed);

			if (Rules::CardComparer::FGreaterThan(crdPlayed, crdHighest, _suitTrump))
			{
				crdHighest = crdPlayed;
				iposplHighestCard = ipospl;
			}
		}
		_vecppl[iposplHighestCard]->TakeTrick();
		_iposplStartTrick = iposplHighestCard;

		_veccrdPlayedThisTurn.clear();
	}

	_suitTrump = Suit::suitNil;
}


void Round::DoScoring()
{
	for (int i = 0; i < 4; i++)
		_vecppl[i]->DoScoring();
}

void Round::ShowRoundSummary()
{
	UserController::ClearScreen();
	for (auto itppl : _vecppl)
	{
		cout << itppl->StrName() << ": " << itppl->CTricksWon() << " tricks Won.\n";
	}
	UserController::WaitForUserAcknowledgement();
}
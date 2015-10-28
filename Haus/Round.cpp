#include "Round.h"

#include <vector>
#include "Cards.h"
#include "Debug.h"
#include "Game.h"
#include "Rules.h"

using namespace std;

Round::Round(const vector<Player*>& vecppl, int iposplDealer)
{
	_vecppl = vecppl;
	_iposplDealer = iposplDealer;
}

Round::~Round()
{
	SafeDelete(_prndvw);
}


void Round::Do()
{
	_dck.Reset();
	_dck.Shuffle();
	DealCards();
	DoBet();
	PlayTricks();
	DoScoring();
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
	Suit suitTrump = _vecppl[_iposplDealer]->SuitChooseTrump();

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

			if (Rules::CardComparer::FGreaterThan(crdPlayed, crdHighest, suitTrump))
			{
				crdHighest = crdPlayed;
				iposplHighestCard = ipospl;
			}
		}
		_vecppl[iposplHighestCard]->TakeTrick();
		_iposplStartTrick = iposplHighestCard;

		_veccrdPlayedThisTurn.clear();
	}
}


void Round::DoScoring()
{
	for (int i = 0; i < 4; i++)
		_vecppl[i]->DoScoring();
}
#include "Round.h"

#include <vector>
#include "Debug.h"
#include "Game.h"

using namespace std;

Round::Round(const vector<Player*>& vecppl, int iposplDealer)
{
	_vecppl = vecppl;
	_iposplDealer = iposplDealer;
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
	for (int i = 0; i < Game::ccrdsPerPl * Game::cpl; i++)
	{
		int ipos = (i + _iposplDealer + 1) % Game::cpl;
		ASSERT(ipos >= 0 && ipos < Game::cpl);
		ASSERTIMPLIES(i == (Game::cpl - 1), ipos == _iposplDealer);
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

	for (int i = 0; i < Game::cpl; i++)
	{
		ipos = (i + _iposplDealer) % Game::cpl;
		ASSERT(ipos < Game::cpl && ipos >= 0);
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
}


void Round::PlayTricks()
{
	// TODO
	NOTREACHED();
}


void Round::DoScoring()
{
	for (int i = 0; i < 4; i++)
		_vecppl[i]->DoScoring();
}
#include "Game.h"
#include <iostream>
#include "Debug.h"
#include "Player.h"

using namespace std;

Game::Game()
{
	int userPos = rand() % 4;
	posplDealer = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		if (i == userPos)
			pl[i] = (Player*)new User();
		else
			pl[i] = (Player*)new Computer(i);
	}
}


Game::~Game()
{
	for (int i = 0; i < 4; i++)
		delete pl[i];
}


void Game::Play()
{
	do
	{
		DoRound();
	} while (FWinnerExists());
	DisplayWinner();
}


void Game::DealCards()
{
	for (int i = 0; i < ccrdsPerPl * cpl; i++)
	{
		int ipos = (i + posplDealer + 1) % cpl;
		ASSERT(ipos >= 0 && ipos < cpl);
		ASSERTIMPLIES(i == (cpl-1), ipos == posplDealer);
		pl[ipos]->TakeCard(_dck.Deal());
	}
}


bool Game::FWinnerExists()
{
	for (int i = 0; i < 4; i++)
	{
		if (pl[i]->FIsWinner())
			return true;
	}
	return false;
}


void Game::DisplayWinner()
{
	int nMax = 0;
	for (int i = 0; i < 4; i++)
	{
		nMax = pl[i]->NScore() > nMax ? pl[i]->NScore() : nMax;
	}

	for (int i = 0; i < 4; i++)
	{
		if (nMax == pl[i]->NScore())
		{
			cout << pl[i]->StrName() << " wins with a score of " << nMax << endl;
		}
	}
}


void Game::DoBet()
{
	BetChoice bcCurHighestBid = BetChoice::DEFAULT_DEALER;
	int iposCurHighBidder = posplDealer;
	BetChoice bcCurBet;
	int ipos;
	bool fForcedToTake = true;

	for (int i = 0; i < cpl; i++)
	{
		ipos = (i + posplDealer) % cpl;
		ASSERT(ipos < cpl && ipos >= 0);
		bcCurBet = pl[ipos]->BcRequestDesiredBet();
		if (bcCurBet > bcCurHighestBid)
		{
			bcCurHighestBid = bcCurBet;
			iposCurHighBidder = ipos;
			fForcedToTake = false;
		}
	}

	// now we know who will have to bet
	pl[iposCurHighBidder]->ForceBet(bcCurHighestBid, fForcedToTake);
}


void Game::DoRound()
{
	_dck.Reset();
	_dck.Shuffle();
	DealCards();
	DoBet();
	PlayTricks();
	DoScoring();
	posplDealer++;
}


void Game::PlayTricks()
{
	// TODO
	NOTREACHED();
}


void Game::DoScoring()
{
	for (int i = 0; i < 4; i++)
		pl[i]->DoScoring();
}


void Game::DetermineWinner(){}
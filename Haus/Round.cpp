#include "Round.h"

#include <assert.h>
#include <vector>
#include <iostream>
#include "Cards.h"
#include "Game.h"
#include "Player.h"
#include "Rules.h"
#include <algorithm>


Round::Round(std::vector<std::unique_ptr<Player>>& vecpopl, int iposplDealer)
	: _rndvw(&_veccrdPlayed, &_veccrdPlayedThisTurn, &_optsuitTrump)
	, _vecpopl(vecpopl)
{
	_iposplDealer = iposplDealer;
	_iposplStartTrick = (_iposplDealer + 1) % Rules::cpl;
}


void Round::Do()
{
	ResetRound();
	DealCards();
	DoBet();
	PlayTricks();
	DoScoring();
	ShowRoundSummary();
}

void Round::ResetRound()
{
	_dck = {};
	for (auto &popl : _vecpopl)
		popl->ResetRound();
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
		assert(ipos >= 0 && ipos < Rules::cpl);
		_vecpopl[ipos]->TakeCard(_dck.Deal());
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
		assert(ipos < Rules::cpl && ipos >= 0);
		bcCurBet = _vecpopl[ipos]->BcRequestDesiredBet();
		if (bcCurBet > bcCurHighestBid)
		{
			bcCurHighestBid = bcCurBet;
			iposCurHighBidder = ipos;
			fForcedToTake = false;
		}
	}

	// now we know who will have to bet
	_vecpopl[iposCurHighBidder]->ForceBet(bcCurHighestBid, fForcedToTake);
	_iposplStartTrick = iposCurHighBidder;
}


void Round::PlayTricks()
{
	this->_optsuitTrump = _vecpopl[_iposplDealer]->SuitChooseTrump();

	for (int itrk = 0; itrk < Rules::ccrdsPerPl; itrk++)
	{
		int iposplHighestCard = _iposplStartTrick;
		Card crdLead = _vecpopl[_iposplStartTrick]->CrdPlay(nullptr);
		Card crdHighest = crdLead;
		_veccrdPlayed.push_back(crdLead);
		_veccrdPlayedThisTurn.push_back(crdLead);
		for (int i = 1; i < Rules::cpl; i++)
		{
			int ipospl = (_iposplStartTrick + i) % Rules::cpl;
			assert(ipospl >= 0 && ipospl < Rules::cpl);
			Card crdPlayed = _vecpopl[ipospl]->CrdPlay(&crdLead);
			_veccrdPlayedThisTurn.push_back(crdPlayed);
			_veccrdPlayed.push_back(crdPlayed);

			if (Rules::CardComparer::FGreaterThan(crdPlayed, crdHighest, *_optsuitTrump))
			{
				crdHighest = crdPlayed;
				iposplHighestCard = ipospl;
			}
		}
		_vecpopl[iposplHighestCard]->TakeTrick();
		_iposplStartTrick = iposplHighestCard;
		
		std::ranges::for_each(_vecpopl, [&](const std::unique_ptr<Player> &popl) noexcept { popl->ShowTurnSummary(
			_vecpopl[iposplHighestCard]->StrName(),
			crdHighest); });


		_veccrdPlayedThisTurn.clear();
	}
}


void Round::DoScoring()
{
	for (auto &popl : _vecpopl)
		popl->DoScoring();
}

void Round::ShowRoundSummary()
{
	UserController::ClearScreen();
	const auto fnDisplayPlayer = [](const auto& popl)
	{
		std::cout
			<< popl->StrName() << ": "
			<< popl->CTricksWon() << " tricks Won. Total Points: " \
			<< popl->NScore() << '\n';
	};
	
	std::ranges::for_each(_vecpopl, fnDisplayPlayer);
	UserController::WaitForUserAcknowledgement();
}
#include "Game.h"
#include <algorithm>
#include <iostream>
#include "Player.h"
#include "Rules.h"


Game::Game(std::vector<std::unique_ptr<Player>> &&vecpopl, GameView *pgmvw)
	: _pgmvw(pgmvw)
{
	_pgmvw->AttachGame(this);

	_vecpopl.reserve(Rules::cpl);
	std::ranges::move(vecpopl, std::back_inserter(_vecpopl));
	std::generate_n(std::inserter(_vecpopl, _vecpopl.end()), Rules::cpl - 1, []() { return std::make_unique<Computer>(0); });
	std::ranges::for_each(_vecpopl, [this](auto &popl){ popl->AttachGameView(_pgmvw);});
}



void Game::Play()
{
	do
	{
		DoRound();
	} while (!FWinnerExists());
	DisplayWinner();
}


bool Game::FWinnerExists()
{
	return std::ranges::any_of(_vecpopl, [](const auto& popl) noexcept { return popl->FIsWinner(); });
}


void Game::DisplayWinner()
{
	auto fnComparePlByNScore = [](const auto& popl1, const auto& popl2) noexcept { return popl1->NScore() > popl2->NScore(); };
	auto itpoplWinner = max_element(_vecpopl.begin(), _vecpopl.end(), fnComparePlByNScore);
	std::cout << (*itpoplWinner)->StrName() << " wins with a score of " << (*itpoplWinner)->NScore() << '\n';
}


void Game::DoRound()
{
	Round rnd(_vecpopl, _iposplDealer);
	_pgmvw->AttachPrndvw(rnd.PrndvwGet());
	rnd.Do();
	_pgmvw->DetachPrndvw();
	_iposplDealer++;
	_iposplDealer = _iposplDealer % Rules::cpl;
}


void Game::DetermineWinner(){}
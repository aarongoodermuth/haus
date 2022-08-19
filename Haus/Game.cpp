#include "Game.h"
#include <algorithm>
#include <iostream>
#include "Player.h"
#include "Rules.h"



using namespace std;

Game::Game(const std::vector<std::unique_ptr<User>> &vecpopl)
	: _vecpopl(vecpopl)
{
	_vecpopl.reserve(Rules::cpl);
	std::ranges::copy(vecpopl, std::back_inserter(_vecpopl));
	std::generate(_vecpopl.end(), _vecpopl.begin() + Rules::cpl, []() { return std::make_unique<Computer>(0); });
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
	cout << (*itpoplWinner)->StrName() << " wins with a score of " << (*itpoplWinner)->NScore() << '\n';
}


void Game::DoRound()
{
	Round rnd(_vecpopl, _iposplDealer);
	_gmvw.AttachPrndvw(rnd.PrndvwGet());
	rnd.Do();
	_gmvw.DetachPrndvw();
	_iposplDealer++;
	_iposplDealer = _iposplDealer % Rules::cpl;
}


void Game::DetermineWinner(){}
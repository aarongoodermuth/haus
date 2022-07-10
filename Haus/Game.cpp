#include "Game.h"
#include <iostream>
#include "Debug.h"
#include "Player.h"
#include "Rules.h"
#include <algorithm>


using namespace std;

Game::Game() : _gmvw()
{
	int userPos = rand() % 4;
	_iposplDealer = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		if (i == userPos)
			_vecpopl.push_back(std::make_unique<User>(&_gmvw));
		else
			_vecpopl.push_back(std::make_unique<Computer>(i));
	}
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
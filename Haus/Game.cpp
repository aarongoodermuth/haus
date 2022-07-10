#include "Game.h"
#include <iostream>
#include "Debug.h"
#include "Player.h"
#include "Rules.h"

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
	for (int i = 0; i < 4; i++)
	{
		if (_vecpopl[i]->FIsWinner())
			return true;
	}
	return false;
}


void Game::DisplayWinner()
{
	int nMax = 0;
	for (int i = 0; i < 4; i++)
	{
		nMax = _vecpopl[i]->NScore() > nMax ? _vecpopl[i]->NScore() : nMax;
	}

	for (int i = 0; i < 4; i++)
	{
		if (nMax == _vecpopl[i]->NScore())
		{
			cout << _vecpopl[i]->StrName() << " wins with a score of " << nMax << endl;
		}
	}
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
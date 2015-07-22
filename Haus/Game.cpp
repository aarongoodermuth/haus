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
			_vecppl.push_back((Player*)new User(&_gmvw));
		else
			_vecppl.push_back((Player*)new Computer(i));
	}
}

Game::~Game()
{
	ASSERT(_vecppl.size() == Rules::cpl);
	for (int i = 0; i < Rules::cpl; i++)
	{
		delete _vecppl[i];
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
		if (_vecppl[i]->FIsWinner())
			return true;
	}
	return false;
}


void Game::DisplayWinner()
{
	int nMax = 0;
	for (int i = 0; i < 4; i++)
	{
		nMax = _vecppl[i]->NScore() > nMax ? _vecppl[i]->NScore() : nMax;
	}

	for (int i = 0; i < 4; i++)
	{
		if (nMax == _vecppl[i]->NScore())
		{
			cout << _vecppl[i]->StrName() << " wins with a score of " << nMax << endl;
		}
	}
}


void Game::DoRound()
{
	Round rnd(_vecppl, _iposplDealer);
	rnd.Do();
	_iposplDealer++;
	_iposplDealer = _iposplDealer % Rules::cpl;
}


void Game::DetermineWinner(){}
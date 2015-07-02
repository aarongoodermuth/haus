#include "Game.h"
#include <iostream>
#include "Debug.h"
#include "Player.h"

using namespace std;

Game::Game()
{
	int userPos = rand() % 4;
	_iposplDealer = rand() % 4;
	for (int i = 0; i < 4; i++)
	{
		if (i == userPos)
			_vecppl.push_back((Player*)new User());
		else
			_vecppl.push_back((Player*)new Computer(i));
	}
}

Game::~Game()
{
	for (int i = 0; i < _vecppl.size(); i++)
	{
		delete _vecppl[i];
	}
}


void Game::Play()
{
	do
	{
		DoRound();
	} while (FWinnerExists());
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
	_iposplDealer++;
}


void Game::DetermineWinner(){}
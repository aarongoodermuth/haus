#include "GameView.h"
#include <assert.h>
#include <iostream>
#include "Game.h"


GameView::GameView() : _prndvw(nullptr)
{
}

void GameView::AttachGame(const Game* pgm)
{
	_pgm = pgm;
}

void GameView::Show()
{
	if (_prndvw != nullptr)
		_prndvw->Show();
}

void GameView::AttachPrndvw(RoundView* prndvw)
{
	assert(_prndvw == nullptr);
	_prndvw = prndvw;
}

void GameView::DetachPrndvw()
{
	assert(_prndvw != nullptr);
	_prndvw = nullptr;
}
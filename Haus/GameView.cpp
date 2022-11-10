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

void GameView::Show(std::optional<Suit> optsuit)
{
	if (_prndvw != nullptr)
		_prndvw->Show(std::move(optsuit));
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
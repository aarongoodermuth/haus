#include "GameView.h"
#include <assert.h>


GameView::GameView() : _prndvw(nullptr)
{
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
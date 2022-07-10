#include "GameView.h"
#include <assert.h>


GameView::GameView() : _prndvw(nullptr)
{
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
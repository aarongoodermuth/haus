#include "GameView.h"

#include "Haus.h"


GameView::GameView() : _prndvw(pNil)
{
}

void GameView::Show()
{
	if (_prndvw != pNil)
		_prndvw->Show();
}

void GameView::AttachPrndvw(RoundView* prndvw)
{
	ASSERT(_prndvw == pNil);
	_prndvw = prndvw;
}

void GameView::DetachPrndvw()
{
	ASSERT(_prndvw != pNil);
	_prndvw = pNil;
}
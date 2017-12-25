#pragma once
#include "View.h"
#include "RoundView.h"

class GameView : public View
{
public:
	GameView();

	void Show() override;
	void AttachPrndvw(RoundView* prndvw);
	void DetachPrndvw();

private:
	RoundView* _prndvw;
};


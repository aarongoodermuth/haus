#pragma once
#include "View.h"
#include "RoundView.h"

class GameView : public View
{
public:
	GameView();

	void Show(std::optional<Suit> optsuit) override;
	void AttachPrndvw(RoundView* prndvw);
	void DetachPrndvw();

private:
	RoundView* _prndvw;
};


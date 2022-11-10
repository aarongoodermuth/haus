#pragma once
#include "View.h"
#include "RoundView.h"

class Game;

class GameView : public View
{
public:
	GameView();

	void AttachGame(const Game*);

	void Show(std::optional<Suit> optsuit) override;
	void AttachPrndvw(RoundView* prndvw);
	void DetachPrndvw();

private:
	RoundView* _prndvw;
	const Game* _pgm;
};


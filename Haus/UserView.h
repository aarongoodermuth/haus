#pragma once

#include <vector>

#include "View.h"

#include "Bet.h"
#include "Cards.h"
#include "GameView.h"

// forward declarations
class User;

class UserView : public View
{
public:
	UserView(User* pusr, GameView* gmvw);

	void Show() override;

private:
	GameView* _pgmvw;

	int* _pcTricksWon;
	BetChoice* _pbc;
	int* _pcBumps;
	int* _pnScore;
	bool* _pfForcedToBet;
	string* _pstrName;
	vector<Card>* _pveccrdHand;
};


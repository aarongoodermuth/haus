#pragma once
#include "Controller.h"

#include "Bet.h"
#include "UserView.h"

// forward declaration
class User;

class UserController : public Controller
{
public:
	UserController(User* pusr);
	
	BetChoice BcRequestDesiredBet(const BetChoiceInfo* rgbci, int cbci);
private:
	UserView *_pusrvw;
};


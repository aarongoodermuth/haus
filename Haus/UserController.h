#pragma once
#include "Controller.h"
#include "Haus.h"

#include "Bet.h"
#include "UserView.h"

// forward declaration
class User;

class UserController : public Controller
{
public:
	UserController(User* pusr);
	
	BetChoice BcRequestDesiredBet(const BetChoiceInfo* rgbci, int cbci);
	uint IcrdRequestCrdPlay(const Card *pcrdLead, uint ccrd);

private:
	UserView *_pusrvw;
};


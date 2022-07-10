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
	
	BetChoice BcRequestDesiredBet(const std::array<BetChoiceInfo, 6> &rgbci, int cbci);
	unsigned int IcrdRequestCrdPlay(const Card *pcrdLead, unsigned int ccrd);

private:
	UserView *_pusrvw;

public:
	static void ClearScreen();
	static void WaitForUserAcknowledgement();
};


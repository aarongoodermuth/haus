#pragma once
#include "Controller.h"

#include "Bet.h"
#include "UserView.h"

// forward declaration
class User;

class UserController : protected Controller
{
public:
	UserController(User* pusr);
	
	BetChoice BcRequestDesiredBet() override;
	unsigned int IcrdRequestCrdPlay(const Card *pcrdLead) override;

private:
	UserView *_pusrvw;

public:
	static void ClearScreen();
	static void WaitForUserAcknowledgement();
};


#pragma once
#include "Controller.h"

#include "Bet.h"
#include "UserView.h"

// forward declaration
class User;
class Player;

class UserController : protected Controller
{
public:
	UserController(User* pusr) : Controller(static_cast<Player*>(pusr)) {}
	
	BetChoice BcRequestDesiredBet() override;
	unsigned int IcrdRequestCrdPlay(const Card *pcrdLead, unsigned int ccrd);

private:
	UserView *_pusrvw;

public:
	static void ClearScreen();
	static void WaitForUserAcknowledgement();
};


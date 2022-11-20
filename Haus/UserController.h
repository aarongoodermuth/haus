#pragma once
#include "Controller.h"

#include "Bet.h"
#include "UserView.h"
#include <memory>

// forward declaration
class User;

class UserController : protected Controller
{
public:
	UserController(User* pusr, GameView* pgmvw);
	
	BetChoice BcRequestDesiredBet() override;
	unsigned int IcrdRequestCrdPlay(const Card *pcrdLead) override;
	Suit SuitChooseTrump() override;

private:
	std::unique_ptr<UserView> _pousrvw;

public:
	static void ClearScreen();
	static void WaitForUserAcknowledgement();
};


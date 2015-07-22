#include <iostream>

#include "Player.h"
#include "UserController.h"

using namespace std;

UserController::UserController(User* pusr) : Controller(&pusr->_usrvw), _pusrvw(&pusr->_usrvw)
{
}

BetChoice UserController::BcRequestDesiredBet(const BetChoiceInfo* rgbci, int cbci)
{
	int bc;

	_pusrvw->Show();
	cout << "\nPlace a bet\nBet Choices:" << endl;

	for (int ibci = 0; ibci < cbci; ibci++)
	{
		cout << ibci+1 << ". " << rgbci[ibci].str << endl;
	}

	cin >> bc;
	return static_cast<BetChoice>(bc-1);
}
#include <iostream>

#include "Player.h"
#include "UserController.h"

using namespace std;

UserController::UserController(User* pusr) : Controller(&pusr->_usrvw), _pusrvw(&pusr->_usrvw)
{
}

BetChoice UserController::BcRequestDesiredBet(const BetChoiceInfo* rgbci, int cbci)
{
	int bc = bcNil;
	bool fHitOnce = false;

	while (bc < (bcFirst+1) || bc > (bcMac+1))
	{
		if (fHitOnce)
			cout << "\n\nINVALID CHOICE\n";

		_pusrvw->Show();
		cout << "\nPlace a bet\nBet Choices:" << endl;

		for (int ibci = 0; ibci < cbci; ibci++)
		{
			cout << ibci + 1 << ". " << rgbci[ibci].str << endl;
		}

		cin >> bc;
	}
	return static_cast<BetChoice>(bc - 1);
}

uint UserController::IcrdRequestCrdPlay(const Card *pcrdLead, uint ccrd)
{
	uint icrd = -1;
	bool fHitOnce = false;

	while (icrd < 1 || icrd > ccrd)
	{
		if (fHitOnce)
			cout << "\n\nINVALID CHOICE\n";

		_pusrvw->Show();
		cout << "\nPlay a card\n" << endl;

		cin >> icrd;
	}
	return icrd - 1;
}

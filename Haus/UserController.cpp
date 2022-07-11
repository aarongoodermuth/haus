#include <iostream>

#include "Player.h"
#include "UserController.h"
#include "Util.h"

using namespace std;

UserController::UserController(User* pusr) : Controller(&pusr->_usrvw), _pusrvw(&pusr->_usrvw)
{
}

BetChoice UserController::BcRequestDesiredBet(const std::array<BetChoiceInfo, 6> &rgbci, int cbci)
{
	int bc;
	bool fHitOnce = false;

	auto fnValidBc = [](int bc) noexcept { return bc > EnumCast(BetChoice::bcFirst) && bc < EnumCast(BetChoice::bcMac); };

	do
	{
		if (fHitOnce)
			cout << "\n\nINVALID CHOICE\n";

		_pusrvw->Show(std::nullopt); // no suit has been chosen yet
		cout << "\nPlace a bet\nBet Choices:" << endl;

		for (int ibci = 0; ibci < cbci; ibci++)
		{
			cout << ibci + 1 << ". " << rgbci[ibci].sz << endl;
		}

		cin >> bc;
	} while (!fnValidBc(bc));

	ClearScreen();
	return static_cast<BetChoice>(bc - 1);
}

unsigned int UserController::IcrdRequestCrdPlay(const Card *pcrdLead, unsigned int ccrd)
{
	unsigned int icrd = -1;
	bool fHitOnce = false;

	while (icrd < 1 || icrd > ccrd)
	{
		if (fHitOnce)
			cout << "\n\nINVALID CHOICE\n";
		fHitOnce = true;

		_pusrvw->Show(std::nullopt /*TODO*/);
		cout << "\nPlay a card\n" << endl;

		cin >> icrd;
	}
	ClearScreen();
	return icrd - 1;
}

// TODO: this is bad object design. But then again, this whole class is.
void UserController::ClearScreen()
{
#ifdef _WIN32
	system("cls");
#else
#ifdef __linux
	system("clear");
#else
	cout << string(100, '\n');
#endif // __linux
#endif // _WIN32
}


// TODO: this is bad object design. But then again, this whole class is.
void UserController::WaitForUserAcknowledgement()
{
	cout << "Press any key to continue...";
	cin.ignore().get();
	ClearScreen();
}

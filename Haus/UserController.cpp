#include <iostream>

#include "Player.h"
#include "UserController.h"
#include "Util.h"


UserController::UserController(User *pusr, GameView* pgmvw)
	: Controller(static_cast<Player *>(pusr))
	, _pousrvw(std::make_unique<UserView>(pusr, pgmvw))
{
}

BetChoice UserController::BcRequestDesiredBet()
{
	int bc;
	bool fHitOnce = false;

	auto fnValidBc = [](int bc) noexcept { return bc > EnumCast(BetChoice::bcFirst) && bc < EnumCast(BetChoice::bcMac); };

	do
	{
		if (fHitOnce)
			std::cout << "\n\nINVALID CHOICE\n";

		_pousrvw->Show(std::nullopt); // no suit has been chosen yet
		std::cout << "\nPlace a bet\nBet Choices:" << std::endl;

		for (int ibci = 0; ibci < sizeof(Player::c_rgbci) / sizeof(BetChoiceInfo); ibci++)
		{
			std::cout << ibci + 1 << ". " << Player::c_rgbci[ibci].sz << std::endl;
		}

		std::cin >> bc;
	} while (!fnValidBc(bc));

	ClearScreen();
	return static_cast<BetChoice>(bc - 1);
}

unsigned int UserController::IcrdRequestCrdPlay(const Card *pcrdLead)
{
	unsigned int icrd = -1;
	bool fHitOnce = false;

	while (icrd < 1 || icrd > _ppl->_veccrdHand.size())
	{
		if (fHitOnce)
			std::cout << "\n\nINVALID CHOICE\n";
		fHitOnce = true;

		_pousrvw->Show(std::nullopt /*TODO*/);
		std::cout << "\nPlay a card\n" << std::endl;

		std::cin >> icrd;
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
	std::cout << "Press any key to continue...";
	std::cin.ignore().get();
	ClearScreen();
}

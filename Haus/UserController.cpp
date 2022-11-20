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
	char chBetChoice;
	bool fHitOnce = false;

	auto fnFBetween = [](auto val, auto low, auto high) noexcept { return val >= low && val <= high; };
	auto fnFValidBc = [&](BetChoice bc) noexcept { return fnFBetween(bc, BetChoice::bcFirst, BetChoice::bcMac); };
	auto fnFValidCh = [&](char chBetChoice) noexcept { return fnFBetween(chBetChoice, '1', '1' + sizeof(BetChoiceInfo)); };
	auto fnBcFromCh = [](char ch) noexcept -> BetChoice { return static_cast<BetChoice>(ch - '1'); };

	do
	{
		if (fHitOnce)
			std::cout << "\n\nINVALID CHOICE\n";
		fHitOnce = true;

		_pousrvw->Show();
		std::cout << "\nPlace a bet\nBet Choices:" << std::endl;

		for (int ibci = 0; ibci < sizeof(Player::c_rgbci) / sizeof(BetChoiceInfo); ibci++)
		{
			std::cout << ibci + 1 << ". " << Player::c_rgbci[ibci].sz << std::endl;
		}

		std::cin >> chBetChoice;

	} while (!fnFValidCh(chBetChoice) || !fnFValidBc(fnBcFromCh(chBetChoice)));

	ClearScreen();
	return static_cast<BetChoice>(chBetChoice - '0');
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

		_pousrvw->Show();
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

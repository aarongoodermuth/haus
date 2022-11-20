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

	auto fnFValidBc = [&](BetChoice bc) noexcept { return FBetween(bc, BetChoice::bcFirst, BetChoice::bcMac); };
	auto fnFValidCh = [&](char chBetChoice) noexcept { return FBetween(chBetChoice, '1', static_cast<char>('1' + sizeof(BetChoiceInfo))); };
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
	return static_cast<BetChoice>(chBetChoice - '1');
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
	return icrd - 1;
}


Suit UserController::SuitChooseTrump()
{

	//TODO: This function shares a lot of similarities with UserController::BcRequestDesiredBet. It should be templatized.
	auto fnFValidSuit = [&](Suit suit) noexcept { return FBetween(suit, Suit::suitFirst, Suit::suitLim); };
	auto fnFValidCh = [&](char chSuit) noexcept { return FBetween(chSuit, '1', static_cast<char>('0' + sizeof(Suit::suitLim))); };
	auto fnSuitFromCh = [](char ch) noexcept -> Suit { return static_cast<Suit>(ch - '1'); };


	_pousrvw->Show();
	std::cout << "\n\n";
	std::cout << "Choose Trump Suit\n";
	std::cout	<< "1. Heart\n"
				<< "2. Diamond\n"
				<< "3. Club\n"
				<< "4. Spade\n";
	// TODO: get string from enum value rather than re-encode the ordinality.

	char chTrumpSuit;
	do
	{
		std::cin >> chTrumpSuit;
	} while (!fnFValidCh(chTrumpSuit) || !fnFValidSuit(fnSuitFromCh(chTrumpSuit)));
	ClearScreen();
	return fnSuitFromCh(chTrumpSuit);
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

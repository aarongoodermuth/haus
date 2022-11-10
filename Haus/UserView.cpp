#include "UserView.h"

#include <iostream>
#include "Player.h"
#include "Util.h"


UserView::UserView(User* pusr, GameView* gmvw) : _pgmvw(gmvw)
{
	_pbc = &pusr->_bc;
	_pcBumps = &pusr->_cBumps;
	_pfForcedToBet = &pusr->_fForcedToBet;
	_pnScore = &pusr->_nScore;
	_pstrName = &pusr->_strName;
	_pveccrdHand = &pusr->_veccrdHand;
	_pcTricksWon = &pusr->_cTricksWon;
}

void UserView::Show()
{
	_pgmvw->Show();

	if (*_pbc != BetChoice::PASS)
	{
		std::cout << "Your Bet: " << EnumCast(*_pbc) << " " << (*_pfForcedToBet ? "Under" : "") << std::endl
			<< "Tricks Won: " << *_pcTricksWon<< std::endl;
	}

	std::cout << "Your Hand: ";
	for (size_t icrd = 0; icrd < _pveccrdHand->size(); icrd++)
		std::cout << std::to_string(icrd+1) << ". " << _pveccrdHand->at(icrd).StrAscii() << "   ";
	std::cout << std::endl;
}


void UserView::ClearScreen()
{
	std::cout << std::string(100, '\n');
}

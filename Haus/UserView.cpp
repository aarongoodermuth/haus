#include "UserView.h"

#include <iostream>
#include "Player.h"
#include "Util.h"

using namespace std;

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

void UserView::Show(std::optional<Suit> optsuit)
{
	_pgmvw->Show(optsuit);

	if (*_pbc != BetChoice::PASS)
	{
		cout << "Your Bet: " << EnumCast(*_pbc) << " " << (*_pfForcedToBet ? "Under" : "") << endl
			<< "Tricks Won: " << *_pcTricksWon<< endl;
	}

	cout << "Your Hand: ";
	for (size_t icrd = 0; icrd < _pveccrdHand->size(); icrd++)
		cout << to_string(icrd+1) << ". " << _pveccrdHand->at(icrd).SzAscii() << "   ";
	cout << endl;
}


void UserView::ClearScreen()
{
	cout << string(100, '\n');
}

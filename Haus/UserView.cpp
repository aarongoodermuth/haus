#include "UserView.h"

#include "Debug.h"
#include "Player.h"

#include <iostream>

using namespace std;

UserView::UserView(User* pusr, GameView* gmvw) : _pgmvw(gmvw)
{
	_pbc = &pusr->_bc;
	_pcBumps = &pusr->_cBumps;
	_pfForcedToBet = &pusr->_fForcedToBet;
	_pnScore = &pusr->_nScore;
	_pstrName = &pusr->_strName;
	_pveccrdHand = &pusr->_veccrdHand;
}

void UserView::Show()
{
	_pgmvw->Show();

	if (*_pbc != BetChoice::PASS && *_pbc != BetChoice::bcNil)
	{
		cout << "Your Bet: " << *_pbc << " " << (*_pfForcedToBet ? "Under" : "") << endl
			<< "Tricks Won: " << *_pcTricksWon<< endl;
	}

	cout << "Your Hand: ";
	for (size_t icrd = 0; icrd < _pveccrdHand->size(); icrd++)
		cout << _pveccrdHand->at(icrd).StrAscii() << " ";
	cout << endl;
}

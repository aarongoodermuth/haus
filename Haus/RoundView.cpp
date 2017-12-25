#include "RoundView.h"

#include "Haus.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include "Cards.h"

#include "Debug.h"

using namespace std;

RoundView::RoundView(vector<Card>* pveccrdPlayed, vector<Card>* pveccrdPlayedThisTurn, Suit* psuitTrump) : _pveccrdPlayed(pveccrdPlayed), _pveccrdPlayedThisTurn(pveccrdPlayedThisTurn), _psuitTrump(psuitTrump)
{
}

void RoundView::Show()
{
	if (_pveccrdPlayedThisTurn->size() == 0)
		return;

	ASSERT(_psuitTrump != pNil && *_psuitTrump != Suit::suitNil);

	cout << "\n\nROUND INFORMATION\n" <<
		"=================\n" <<
		"Trump: " << StrFromSuit(*_psuitTrump) << "\n" <<
		"Lead Suit: " << StrFromSuit(_pveccrdPlayedThisTurn->at(0).suit) << "\n" <<
		"Played Cards: ";

	for (uint icrd = 0; icrd < _pveccrdPlayed->size(); icrd++)
		cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";

	cout << endl;
}

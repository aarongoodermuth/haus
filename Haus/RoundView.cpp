#include "RoundView.h"

#include "Haus.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include "Cards.h"
#include "Rules.h"

#include "Debug.h"

using namespace std;

RoundView::RoundView(vector<Card>* pveccrdPlayed, vector<Card>* pveccrdPlayedThisTurn, Suit* psuitTrump) : _pveccrdPlayed(pveccrdPlayed), _pveccrdPlayedThisTurn(pveccrdPlayedThisTurn), _psuitTrump(psuitTrump)
{
}

void RoundView::Show()
{
	bool fNoCardsPlayed = (_pveccrdPlayedThisTurn->size() == 0);

	// we don't have enough information about the round yet. Just return.
	if (_psuitTrump == nullptr || *_psuitTrump == Suit::suitNil)
		return;

	// Show all the info that is about the whole round
	cout << "ROUND INFORMATION\n" <<
		"=================\n" <<
		"Trump: " << StrFromSuit(*_psuitTrump) << "\n" <<
		"Played This Round: ";
	for (uint icrd = 0; icrd < _pveccrdPlayed->size(); icrd++)
		cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";


	// Show all the info that is just relevant to this trickset
	cout << "\n\nTHIS TURN\n" <<
		"=================\n" <<
		"Lead Suit: " << (fNoCardsPlayed ? "(You Choose)" : StrFromSuit(_pveccrdPlayedThisTurn->at(0).suit)) << "\n" <<
		"Played This Turn: ";

	for (uint icrd = (_pveccrdPlayed->size() / Rules::cpl) * Rules::cpl; icrd < _pveccrdPlayed->size(); icrd++)
		cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";

	cout << endl;
}

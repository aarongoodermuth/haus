#include "RoundView.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include "Cards.h"
#include "Rules.h"

using namespace std;

RoundView::RoundView(vector<Card>* pveccrdPlayed, vector<Card>* pveccrdPlayedThisTurn, Suit* psuitTrump) : _pveccrdPlayed(pveccrdPlayed), _pveccrdPlayedThisTurn(pveccrdPlayedThisTurn), _psuitTrump(psuitTrump)
{
}

void RoundView::Show(std::optional<Suit> optsuit)
{
	bool fNoCardsPlayed = (_pveccrdPlayedThisTurn->size() == 0);

	// we don't have enough information about the round yet. Just return.
	if (!optsuit)
		return;

	// Show all the info that is about the whole round
	cout << "ROUND INFORMATION\n" <<
		"=================\n" <<
		"Trump: " << ChFromSuit(*_psuitTrump) << "\n" <<
		"Played This Round: ";
	for (unsigned int icrd = 0; icrd < _pveccrdPlayed->size(); icrd++)
		cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";


	// Show all the info that is just relevant to this trickset
	cout << "\n\nTHIS TURN\n" <<
		"=================\n" <<
		"Lead Suit: " << (fNoCardsPlayed ? "(You Choose)" : std::to_string(ChFromSuit(_pveccrdPlayedThisTurn->at(0).suit))) << "\n" <<
		"Played This Turn: ";

	for (unsigned int icrd = (_pveccrdPlayed->size() / Rules::cpl) * Rules::cpl; icrd < _pveccrdPlayed->size(); icrd++)
		cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";

	cout << endl;
}

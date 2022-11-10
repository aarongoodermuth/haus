#include "RoundView.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include "Cards.h"
#include "Rules.h"


RoundView::RoundView(std::vector<Card>* pveccrdPlayed, std::vector<Card>* pveccrdPlayedThisTurn, Suit* psuitTrump) : _pveccrdPlayed(pveccrdPlayed), _pveccrdPlayedThisTurn(pveccrdPlayedThisTurn), _psuitTrump(psuitTrump)
{
}

void RoundView::Show(std::optional<Suit> optsuit)
{
	bool fNoCardsPlayed = (_pveccrdPlayedThisTurn->size() == 0);

	// Show all the info that is about the whole round
	std::cout << "ROUND INFORMATION\n" <<
		"=================\n" <<
		"Trump: " << ChFromSuit(*_psuitTrump) << "\n";

	// we don't have enough information about the round yet. Just return.
	if (!optsuit)
		return;
	std::cout << "Played This Round: ";
	for (unsigned int icrd = 0; icrd < _pveccrdPlayed->size(); icrd++)
		std::cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";

	// Show all the info that is just relevant to this trickset
	std::cout << "\n\nTHIS TURN\n" <<
		"=================\n" <<
		"Lead Suit: " << (fNoCardsPlayed ? "(You Choose)" : std::to_string(ChFromSuit(_pveccrdPlayedThisTurn->at(0).suit))) << "\n" <<
		"Played This Turn: ";

	for (unsigned int icrd = (_pveccrdPlayed->size() / Rules::cpl) * Rules::cpl; icrd < _pveccrdPlayed->size(); icrd++)
		std::cout << _pveccrdPlayed->at(icrd).StrAscii() << "  ";

	std::cout << std::endl;
}

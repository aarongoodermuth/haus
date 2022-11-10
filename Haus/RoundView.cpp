#include "RoundView.h"

#include <iostream>
#include <string>
#include <stdio.h>
#include "Cards.h"
#include "Rules.h"


RoundView::RoundView(std::vector<Card>* pveccrdPlayed, std::vector<Card>* pveccrdPlayedThisTurn, std::optional<Suit>* poptsuitTrump) 
	: _pveccrdPlayed(pveccrdPlayed)
	, _pveccrdPlayedThisTurn(pveccrdPlayedThisTurn)
	, _poptsuitTrump(poptsuitTrump)
{
}

void RoundView::Show()
{
	bool fNoCardsPlayed = (_pveccrdPlayedThisTurn->size() == 0);

	// Show all the info that is about the whole round
	std::cout	<< "ROUND INFORMATION\n"
				<< "=================\n";

	if (!*_poptsuitTrump)
		return;

	std::cout << "Trump: " << ChFromSuit(_poptsuitTrump->value()) << "\n";

	std::cout << "Played This Round: ";
	for (const Card &crd : *_pveccrdPlayed)
		std::cout <<crd.StrAscii() << "  ";

	const std::string strLeadSuit = [fNoCardsPlayed, this]() -> std::string
	{
		if (fNoCardsPlayed)
			return "(You Choose)";
		
		std::string strRet;
		strRet.push_back(ChFromSuit(_pveccrdPlayedThisTurn->front().suit));
		return strRet;

	}();

	// Show all the info that is just relevant to this trickset
	std::cout << "\n\nTHIS TURN\n" <<
		"=================\n" <<
		"Lead Suit: " << strLeadSuit << "\n" <<
		"Played This Turn: ";

	for (const Card &crd : *_pveccrdPlayedThisTurn)
		std::cout << crd.StrAscii() << "  ";

	std::cout << std::endl;
}

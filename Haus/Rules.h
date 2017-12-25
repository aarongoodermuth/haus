#pragma once

#include "Cards.h"

 struct Rules
{
	 static const int cpl = 4;
	 static const int ccrdsPerPl = 6;

	 static bool FLegalPlay(Card crdToPlay, vector<Card> veccrdHand, Card* pcrdLead);

	 class CardComparer
	 {
	 public:
		 static bool FGreaterThan(Card crdLeft, Card crdRight, Suit suitTrump);

	 private:
		 static int NTrumpValuation(Card crd, Suit suitTrump);
	 };

private:
	 static bool FTrump(Card crd, Suit suitTrump);
	 static bool FRightBar(Card crd, Suit suitTrump);
	 static bool FLeftBar(Card crd, Suit suitTrump);
};


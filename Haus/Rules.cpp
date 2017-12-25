#include "Rules.h"

#include "Debug.h"
#include "Haus.h"

bool Rules::FLegalPlay(Card crdToPlay, vector<Card> veccrdHand, Card* pcrdLead)
{
	if (pcrdLead == NULL || crdToPlay.suit == pcrdLead->suit)
		return true;

	int ccrd = veccrdHand.size();
	for (int icrd = 0; icrd < ccrd; icrd++)
	{
		if (veccrdHand[icrd].suit == pcrdLead->suit)
			return false;
	}

	return true;
}


bool Rules::CardComparer::FGreaterThan(Card crdLeft, Card crdRight, Suit suitTrump)
{
	ASSERT(!(crdLeft.suit == crdRight.suit && crdLeft.val == crdRight.val));
	ASSERT(crdLeft.suit != Suit::suitLim && crdLeft.val != Value::valLim);

	// we are always greater than the null card
	if (crdRight.suit == Suit::suitLim || crdRight.val == Value::valLim)
		return true;

	bool fLeftTrump = FTrump(crdLeft, suitTrump);
	bool fRightTrump = FTrump(crdRight, suitTrump);

	if (fLeftTrump && fRightTrump)
	{
		// both are trump
		return NTrumpValuation(crdLeft, suitTrump) > NTrumpValuation(crdRight, suitTrump);
	}
	else if (!fLeftTrump && !fRightTrump)
	{
		// neither are trump
		return crdLeft.val > crdRight.val;
	}
	else
	{
		// only one is trump
		// left > right if left is trump
		return fLeftTrump;
	}
}


int Rules::CardComparer::NTrumpValuation(Card crd, Suit suitTrump)
{
	int nValue = 0;
	switch (crd.val)
	{
	case Value::JACK:
		nValue++;
		if (crd.suit == suitTrump)
			nValue++;
	case Value::ACE:
		nValue++;
	case Value::KING:
		nValue++;
	case Value::QUEEN:
		nValue++;
	case Value::TEN:
		nValue++;
	case Value::NINE:
		break;
	default:
		NOTREACHED();
	}

	return nValue;
}


bool Rules::FTrump(Card crd, Suit suitTrump)
{
	return crd.suit == suitTrump || FLeftBar(crd, suitTrump);
}

bool Rules::FRightBar(Card crd, Suit suitTrump)
{
	return crd.suit == suitTrump && crd.val == Value::JACK;
}


bool Rules::FLeftBar(Card crd, Suit suitTrump)
{
	if (crd.val != Value::JACK)
		return false;

	switch (crd.suit)
	{
	case Suit::CLUB:
		return crd.suit == Suit::SPADE;
	case Suit::SPADE:
		return crd.suit == Suit::CLUB;
	case Suit::HEART:
		return crd.suit == Suit::DIAMOND;
	case Suit::DIAMOND:
		return crd.suit == Suit::HEART;
	default:
		NOTREACHEDRET(false);
	}
}


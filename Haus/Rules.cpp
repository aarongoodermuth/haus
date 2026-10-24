#include "Rules.h"

#include <algorithm>
#include <assert.h>

bool Rules::FLegalPlay(const Card crdToPlay, const std::vector<Card> veccrdHand, const Card* pcrdLead, Suit suitTrump)
{
	if (pcrdLead == nullptr)
		return true;

	// A bar is trump for every purpose, including following suit. The
	// effective suit of a card is trump when FTrump says so, else its
	// printed suit. (Fixed 2026-10-24: the old version compared printed
	// suits, so the left bar escaped following trump, and the right bar
	// escaped following a led left bar.)
	auto suitEffective = [suitTrump](const Card& crd) {
		return FTrump(crd, suitTrump) ? suitTrump : crd.suit;
	};
	const Suit suitLed = suitEffective(*pcrdLead);
	if (suitEffective(crdToPlay) == suitLed)
		return true;

	return std::ranges::none_of(veccrdHand, [suitLed, &suitEffective](const Card& crd) {
		return suitEffective(crd) == suitLed;
	});
}


bool Rules::CardComparer::FGreaterThan(Card crdLeft, Card crdRight, Suit suitTrump)
{
	assert(!(crdLeft.suit == crdRight.suit && crdLeft.val == crdRight.val));
	assert(crdLeft.suit != Suit::suitLim && crdLeft.val != Value::valLim);

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
		assert(false);
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

	switch (suitTrump)
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
		assert(false);
		return false;
	}
}


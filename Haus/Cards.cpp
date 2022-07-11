#include "Cards.h"

#include <assert.h>
#include <cstdlib>
#include "Util.h"


/*************/
/*  D E C K  */
/*************/
Deck::Deck()
{
	for (auto suit = EnumCast(Suit::suitFirst); suit < EnumCast(Suit::suitLim); suit++)
	{
		for (auto val = EnumCast(Value::valFirst); val < EnumCast(Value::valLim); val++)
			_veccrd.push_back(Card{static_cast<Suit>(suit), static_cast<Value>(val)});
	}

	Shuffle();
}


void Deck::Shuffle()
{
	constexpr int ccrds = EnumCast(Suit::suitLim) * EnumCast(Value::valLim);

	assert(ccrds == _veccrd.size());

	for (int ipos = 0; ipos < ccrds; ipos++)
	{
		int iposSwap = ipos + rand() % (ccrds - ipos);
		assert(iposSwap >= ipos  && iposSwap <= ccrds);
		Card crdT = _veccrd[ipos];
		Card crdSwap = _veccrd[iposSwap];

		_veccrd[ipos] = crdSwap;
		_veccrd[iposSwap] = crdT;
	}
}


Card Deck::Deal()
{
	Card crdRet = _veccrd.back();
	_veccrd.pop_back();
	return crdRet;
}
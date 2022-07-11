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
	for (int ipos = 0; ipos < c_ccrds; ipos++)
	{
		int iposSwap = ipos + rand() % (c_ccrds - ipos);
		assert(iposSwap >= ipos  && iposSwap <= c_ccrds);

		std::swap(_veccrd[ipos], _veccrd[iposSwap]);
	}
}


Card Deck::Deal()
{
	Card crdRet = _veccrd.back();
	_veccrd.pop_back();
	return crdRet;
}
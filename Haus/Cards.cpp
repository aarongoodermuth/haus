#include "Cards.h"

#include <assert.h>
#include <cstdlib>
#include "Util.h"


/*************/
/*  C A R D  */
/*************/
Card::Card(Suit suitT, Value valT)
{
	suit = suitT;
	val = valT;
}


Card::Card()
{
	suit = Suit::suitLim;
	val = Value::valLim;
}


/*************/
/*  D E C K  */
/*************/
Deck::Deck()
{
	for (int suit = (int)Suit::suitFirst; suit < (int)Suit::suitLim; suit++)
	{
		for (int val = (int)Value::valFirst; val < (int)Value::valLim; val++)
		{
			_veccrd.push_back(Card((Suit)suit, (Value)val));
		}
	}

	Shuffle();
}


void Deck::Shuffle()
{
	const int ccrds = (int)Suit::suitLim * (int)Value::valLim;
	int iposSwap;

	assert(ccrds == _veccrd.size());

	for (int ipos = 0; ipos < ccrds; ipos++)
	{
		iposSwap = ipos + rand() % (ccrds - ipos);
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
#include "Cards.h"

#include <cstdlib>

#include "Debug.h"


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

static const string _mpvalstr[(int)Value::valLim] = { "9", "10", "J", "Q", "K", "A" };
#ifdef _WIN32
static const string _mpsuitstr[(int)Suit::suitLim] = { "\3" /*heart*/, "\4" /*diamond*/, "\5" /*club*/, "\6" /*spade*/ };
#else
#ifdef __linus
static const string _mpsuitstr[(int)Suit::suitLim] = { "\u2665" /*heart*/, "\u2666" /*diamond*/, "\u2663" /*club*/, "\u2660" /*spade*/ };
#else
#error "What platform are we on"
#endif // __linux
#endif // _WIN32

string StrFromSuit(Suit suit)
{
	if (suit < Suit::suitFirst || suit > Suit::suitLim)
		NOTREACHEDRET("");
	return _mpsuitstr[(int)suit];
}

string Card::StrAscii()
{
	return _mpvalstr[(int)val] + _mpsuitstr[(int)suit];
}


/*************/
/*  D E C K  */
/*************/
Deck::Deck()
{
	Reset();
}


void Deck::Shuffle()
{
	const int ccrds = (int)Suit::suitLim * (int)Value::valLim;
	int iposSwap;

	ASSERT(ccrds == _veccrd.size());

	for (int ipos = 0; ipos < ccrds; ipos++)
	{
		iposSwap = ipos + rand() % (ccrds - ipos);
		ASSERT(iposSwap >= ipos  && iposSwap <= ccrds);
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


void Deck::Reset()
{
	_veccrd.clear();

	for (int suit = (int)Suit::suitFirst; suit < (int)Suit::suitLim; suit++)
	{
		for (int val = (int)Value::valFirst; val < (int)Value::valLim; val++)
		{
			_veccrd.push_back(Card((Suit)suit, (Value)val));
		}
	}
}
#pragma once
#include <vector>
#include <string>
#include "Util.h"


enum class Suit : int
{
	HEART = 0,
	suitFirst = 0,
	DIAMOND,
	CLUB,
	SPADE,
	suitLim
};


enum class Value : int
{
	NINE = 0,
	valFirst = NINE,
	TEN,
	JACK,
	QUEEN,
	KING,
	ACE,
	valLim
};


constexpr static const char* _mpvalstr[EnumCast(Value::valLim)] = { "9", "10", "J", "Q", "K", "A" };
#ifdef _WIN32
constexpr static const char _mpsuitstr[EnumCast(Suit::suitLim)] = { 'H' /*heart*/, 'D' /*diamond*/, 'C' /*club*/, 'S' /*spade*/ };
#else
#ifdef __linux
constexpr static const string _mpsuitstr[(int)Suit::suitLim] = { "\u2665" /*heart*/, "\u2666" /*diamond*/, "\u2663" /*club*/, "\u2660" /*spade*/ };
#else
#error "What platform are we on"
#endif // __linux
#endif // _WIN32

constexpr char ChFromSuit(Suit suit)
{
	return _mpsuitstr[EnumCast(suit)];
}


struct Card
{
	Card(Suit suitT, Value valT);
	Card();
	Suit suit;
	Value val;
	constexpr const char *SzAscii()
	{
		return _mpvalstr[EnumCast(val)] + ChFromSuit(suit);
	}
};


class Deck
{
public:
	Deck();
	
	Card Deal();

private:
	void Shuffle();
	std::vector<Card> _veccrd;
};

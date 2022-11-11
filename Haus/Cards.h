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
constexpr static const char _mpsuitch[(int)Suit::suitLim] = { '\5' /*heart*/, '\4' /*diamond*/, '\3' /*club*/, '\6' /*spade*/ };

constexpr char ChFromSuit(Suit suit)
{
	return _mpsuitch[EnumCast(suit)];
}


struct Card
{
	Suit suit;
	Value val;
	std::string StrAscii() const;
};


class Deck
{
public:
	Deck();
	
	Card Deal();

private:
	static constexpr size_t c_ccrds = EnumCast(Suit::suitLim) * EnumCast(Value::valLim);

	void Shuffle();
	std::vector<Card> _veccrd;
};

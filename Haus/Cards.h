#pragma once
#include <vector>

using namespace std;

enum class Suit
{
	HEART = 0,
	suitFirst = 0,
	DIAMOND,
	CLUB,
	SPADE,
	suitLim
};


enum class Value
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


struct Card
{
	Card(Suit suitT, Value valT);
	Suit suit;
	Value val;
};


class Deck
{
public:
	Deck();

	void Shuffle();
	Card Deal();
	void Reset();

private:
	vector<Card> _veccrd;
};

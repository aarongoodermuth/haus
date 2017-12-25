#pragma once
#include <vector>

using namespace std;

enum class Suit
{
	suitNil = -1,
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

string StrFromSuit(Suit suit);

struct Card
{
	Card(Suit suitT, Value valT);
	Card();
	Suit suit;
	Value val;
	string StrAscii();
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

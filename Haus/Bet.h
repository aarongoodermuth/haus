#pragma once

#include <string>

using namespace std;

enum BetChoice // bc
{
	bcNil = -1,
	bcFirst = 0,
	PASS = 0,
	THREE,
	DEFAULT_DEALER = THREE,
	FOUR,
	FIVE,
	SIX,
	TWELVE,
	bcMac = TWELVE,
	cbc
};


struct BetChoiceInfo
{
	BetChoice bc;
	int nBetAmount;
	int nRequiredTricks;
	string str;
};
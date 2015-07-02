#pragma once

#include <string>

using namespace std;

enum BetChoice // bc
{
	bcNil = -1,
	bcFirst = 0,
	PASS = 0,
	FOUR,
	DEFAULT_DEALER = FOUR,
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

/*****************/
/*  P L A Y E R  */
/*****************/
class Player
{
public:
	void ResetRound();
	virtual BetChoice BcRequestDesiredBet() = 0;
	void ForceBet(BetChoice, bool fForcedToBet);
	void DoScoring();
	bool FIsWinner();

	// quick and dirty
	inline void TakeTrick(){ _cTricksWon++; }

	// Getters
	inline int NScore(){ return _nScore; }
	inline string StrName() { return _strName; }

protected:
	void Init();

	int _cTricksWon;
	BetChoice _bc;
	int _cBumps;
	int _nScore;
	bool _fForcedToBet;
	string _strName;

	static const int c_cBetChoices = (int)BetChoice::cbc;
	static const BetChoiceInfo c_rgbci[c_cBetChoices];
};


/*********************/
/*  C O M P U T E R  */
/*********************/
class Computer : Player
{
public:
	Computer(int iPos);

	BetChoice BcRequestDesiredBet() override;
	
private: 
	static const string c_rgstrNames[4];
};


/*************/
/*  U S E R  */
/*************/
class User : Player
{
public:
	User();

	BetChoice BcRequestDesiredBet() override;
};
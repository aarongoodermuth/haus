#pragma once

#include <string>
#include <vector>
#include "Bet.h"
#include "Cards.h"
#include "UserController.h"
#include "UserView.h"

using namespace std;

//forward declarations
class GameView;

/*****************/
/*  P L A Y E R  */
/*****************/
class Player
{
public:
	void ResetRound();
	virtual BetChoice BcRequestDesiredBet() = 0;
	virtual Card CrdPlay(Card* pcrdLead) = 0;
	virtual Suit SuitChooseTrump() = 0;
	void ForceBet(BetChoice, bool fForcedToBet);
	void TakeCard(const Card &crd);
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
	vector<Card> _veccrdHand;

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
	Card CrdPlay(Card* pcrdLead) override;
	Suit SuitChooseTrump() override;

private:
	static const string c_rgstrNames[4];
};


/*************/
/*  U S E R  */
/*************/
class User : Player
{
	friend class UserView;
	friend class UserController;

public:
	User(GameView* pgmvw);

	BetChoice BcRequestDesiredBet() override;
	Card CrdPlay(Card* pcrdLead) override;
	Suit SuitChooseTrump() override;

protected:
	UserController _usrcnt;
	UserView _usrvw;
};
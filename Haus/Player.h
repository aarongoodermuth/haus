#pragma once

#include <array>
#include <string>
#include <vector>
#include "Bet.h"
#include "Cards.h"
#include "UserController.h"
#include "Controller.h"
#include "UserView.h"
#include <memory>


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

	void AttachGameView(GameView* pgmvw);

	// quick and dirty
	inline void TakeTrick(){ _cTricksWon++; }

	// Getters
	inline int NScore(){ return _nScore; }
	inline std::string StrName() { return _strName; }
	inline int CTricksWon() { return _cTricksWon; }

protected:
	void Init();

	int _cTricksWon = 0;
	BetChoice _bc = BetChoice::PASS;
	int _cBumps = 0;
	int _nScore = 0;
	bool _fForcedToBet = false;
	std::string _strName;
	std::vector<Card> _veccrdHand;
	GameView* _pgmvw;

	static const int c_cBetChoices = (int)BetChoice::cbc;
	static constexpr const std::array<BetChoiceInfo, 6> c_rgbci = {
	BetChoiceInfo{ BetChoice::PASS, 0 /*nBetAmount*/, 0 /*nRequredTricks*/, "pass" },
	BetChoiceInfo{ BetChoice::THREE, 3 /*nBetAmount*/, 3 /*nRequredTricks*/, "three" },
	BetChoiceInfo{ BetChoice::FOUR, 4 /*nBetAmount*/, 4 /*nRequredTricks*/, "four" },
	BetChoiceInfo{ BetChoice::FIVE, 5 /*nBetAmount*/, 5 /*nRequredTricks*/, "five" },
	BetChoiceInfo{ BetChoice::SIX, 6 /*nBetAmount*/, 6 /*nRequredTricks*/, "six" },
	BetChoiceInfo{ BetChoice::TWELVE, 12 /*nBetAmount*/, 6 /*nRequredTricks*/, "haus" }
	};

	friend class Controller;
	friend class BrainlessController;
	friend class UserController;
};


/*********************/
/*  C O M P U T E R  */
/*********************/
class Computer : public Player
{
public:
	Computer(int iPos);

	BetChoice BcRequestDesiredBet() override;
	Card CrdPlay(Card* pcrdLead) override;
	Suit SuitChooseTrump() override;

private:
	static constexpr const std::array<const char*, 4> c_rgszNames = { "Bender", "Karen", "Jarvis", "HAL" };
};


/*************/
/*  U S E R  */
/*************/
class User : public Player
{
	friend class UserView;
	friend class UserController;

public:
	User();

	BetChoice BcRequestDesiredBet() override;
	Card CrdPlay(Card* pcrdLead) override;
	Suit SuitChooseTrump() override;

protected:
	std::unique_ptr<UserController> _pousrcnt = std::make_unique<UserController>(this);
	std::unique_ptr<View> _pousrvw = std::make_unique<NoView>();
};
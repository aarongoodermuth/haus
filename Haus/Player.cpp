#include "Player.h"
#include "Debug.h"

const string Computer::c_rgstrNames[4] = { "Bender", "Karen", "Jarvis", "HAL" };
const BetChoiceInfo Player::c_rgbci[Player::c_cBetChoices] =
{
	{ BetChoice::PASS, 0 /*nBetAmount*/, 0 /*nRequredTricks*/, "pass" },
	{ BetChoice::FOUR, 4 /*nBetAmount*/, 4 /*nRequredTricks*/, "four" },
	{ BetChoice::PASS, 5 /*nBetAmount*/, 5 /*nRequredTricks*/, "five" },
	{ BetChoice::PASS, 6 /*nBetAmount*/, 6 /*nRequredTricks*/, "six" },
	{ BetChoice::PASS, 12 /*nBetAmount*/, 6 /*nRequredTricks*/, "haus" }
};

/*****************/
/*  P L A Y E R  */
/*****************/
void Player::ResetRound()
{
	_cTricksWon = 0;
	_bc = BetChoice::PASS;
	_fForcedToBet = false;
}

void Player::ForceBet(BetChoice bc, bool fForcedToBet)
{
	_bc = bc;
	_fForcedToBet = fForcedToBet;
}

void Player::DoScoring()
{
	ASSERT(_bc != bcNil && _bc >= bcFirst && _bc <= bcMac);

	int nRoundBet = c_rgbci[_bc].nRequiredTricks;
	if (_cTricksWon >= nRoundBet)
	{
		_nScore += _cTricksWon;
	}
	else
	{
		_nScore -= nRoundBet;
		_cBumps++;
		if (_fForcedToBet)
			_cBumps++;
	}

	ResetRound();
}

bool Player::FIsWinner()
{
	return _nScore > 31; // TODO: refactor: duh!
}

void Player::Init()
{
	_nScore = 0;
	_cTricksWon = 0;
	_cBumps = 0;
	ResetRound();
}


/*********************/
/*  C O M P U T E R  */
/*********************/
Computer::Computer(int iPos)
{
	_strName = c_rgstrNames[iPos];
	Init();
}

BetChoice Computer::BcRequestDesiredBet()
{
	return static_cast<BetChoice>(rand() % BetChoice::cbc);
}


/*************/
/*  U S E R  */
/*************/
User::User()
{
	_strName = "You";
	Init();
}

BetChoice User::BcRequestDesiredBet()
{
	// TODO
	return BetChoice::PASS; //temp
	/*
	int betDesired = 0;

	if (betDesired == 0)
	return 0;

	while (true)
	{
	if (betDesired == 0)
	return 0;

	if (betDesired > currentHighest && IsValidBet())
	{

	}
	}
	}*/
}
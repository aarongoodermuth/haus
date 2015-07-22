#include "Player.h"

#include "Debug.h"
#include "Rules.h"

const string Computer::c_rgstrNames[4] = { "Bender", "Karen", "Jarvis", "HAL" };
const BetChoiceInfo Player::c_rgbci[Player::c_cBetChoices] =
{
	{ BetChoice::PASS, 0 /*nBetAmount*/, 0 /*nRequredTricks*/, "pass" },
	{ BetChoice::THREE, 3 /*nBetAmount*/, 3 /*nRequredTricks*/, "three" },
	{ BetChoice::FOUR, 4 /*nBetAmount*/, 4 /*nRequredTricks*/, "four" },
	{ BetChoice::FIVE, 5 /*nBetAmount*/, 5 /*nRequredTricks*/, "five" },
	{ BetChoice::SIX, 6 /*nBetAmount*/, 6 /*nRequredTricks*/, "six" },
	{ BetChoice::TWELVE, 12 /*nBetAmount*/, 6 /*nRequredTricks*/, "haus" }
};

/*****************/
/*  P L A Y E R  */
/*****************/
void Player::ResetRound()
{
	_cTricksWon = 0;
	_bc = BetChoice::PASS;
	_fForcedToBet = false;
	_veccrdHand.clear();
}

void Player::ForceBet(BetChoice bc, bool fForcedToBet)
{
	_bc = bc;
	_fForcedToBet = fForcedToBet;
}

void Player::TakeCard(Card &crd)
{
	_veccrdHand.push_back(crd);
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
	return _nScore >= 31; // TODO: refactor: duh!
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
	// TODO
	return BetChoice::PASS; //temp
	return static_cast<BetChoice>(rand() % BetChoice::cbc);
}


Card Computer::CrdPlay(Card* pcrdLead)
{
	// TODO
	int iposcrd = -1;
	Card crdRet;
	do
	{
		crdRet = _veccrdHand[++iposcrd];
	} while (!Rules::FLegalPlay(crdRet, _veccrdHand, pcrdLead));
	_veccrdHand.erase(_veccrdHand.begin() + iposcrd);
	return crdRet;
}


Suit Computer::SuitChooseTrump()
{
	// TODO
	int iposcrd = 0;
	return _veccrdHand[iposcrd].suit;
}


/*************/
/*  U S E R  */
/*************/
User::User(GameView* pgmvw) : _usrvw(this, pgmvw), _usrcnt(this)
{
	_strName = "You";
	Init();
}

BetChoice User::BcRequestDesiredBet()
{
	return _usrcnt.BcRequestDesiredBet(c_rgbci, c_cBetChoices);
}

Card User::CrdPlay(Card* pcrdLead)
{
	// TODO
	int iposcrd = -1;
	Card crdRet;
	do
	{
		crdRet = _veccrdHand[++iposcrd];
	} while (!Rules::FLegalPlay(crdRet, _veccrdHand, pcrdLead));
	_veccrdHand.erase(_veccrdHand.begin() + iposcrd);
	return crdRet;
}


Suit User::SuitChooseTrump()
{
	// TODO
	int iposcrd = 0;
	return _veccrdHand[iposcrd].suit;
}
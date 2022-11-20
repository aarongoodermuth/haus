#include "Player.h"

#include "Bet.h"
#include "Rules.h"
#include "Util.h"
#include <assert.h>
#include <iostream>


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

void Player::TakeCard(const Card &crd)
{
	_veccrdHand.push_back(crd);
}

void Player::DoScoring()
{
	int nRoundBet = c_rgbci[EnumCast(_bc)].nRequiredTricks;
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

void Player::AttachGameView(GameView *pgmvw)
{
	_pgmvw = pgmvw;
}


/*********************/
/*  C O M P U T E R  */
/*********************/
Computer::Computer(int iPos)
{
	_strName = c_rgszNames[iPos];
	Init();
}

BetChoice Computer::BcRequestDesiredBet()
{
	// TODO
	return BetChoice::PASS; //temp
	return static_cast<BetChoice>(rand() % EnumCast(BetChoice::cbc));
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
User::User(GameView* pgmvw)
	: _pousrcnt(std::make_unique<UserController>(this, pgmvw))
	, _pousrvw(std::make_unique<UserView>(this, pgmvw))
{
	_strName = "You";
	Init();
}

BetChoice User::BcRequestDesiredBet()
{
	return _pousrcnt->BcRequestDesiredBet();
}

Card User::CrdPlay(Card* pcrdLead)
{
	bool fFirstAsk = true;
	Card crdRet;
	unsigned int icrd;

	do
	{
		if (!fFirstAsk)
			std::cout << std::endl << "That is not a legal card" << std::endl;
		icrd = _pousrcnt->IcrdRequestCrdPlay(pcrdLead);
		crdRet = _veccrdHand[icrd];
		fFirstAsk = false;
	} while (!Rules::FLegalPlay(crdRet, _veccrdHand, pcrdLead));

	_veccrdHand.erase(_veccrdHand.begin() + icrd);
	return crdRet;
}


Suit User::SuitChooseTrump()
{
	// TODO
	int iposcrd = 0;
	return _veccrdHand[iposcrd].suit;
}


void User::ShowTurnSummary(const std::string &strWinnerName, const Card &crdHighestOfRound)
{
	_pousrcnt->ClearScreen();
	_pousrvw->Show();
	_pousrvw->ShowTurnSummary(strWinnerName, crdHighestOfRound);
	_pousrcnt->WaitForUserAcknowledgement();
}
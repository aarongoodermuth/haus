#pragma once

#include "View.h"
#include "Bet.h"
#include "Rules.h"

class Player;

class Controller
{
public:
	Controller(Player* ppl) : _ppl(ppl) {}
	virtual BetChoice BcRequestDesiredBet() = 0;
	virtual unsigned int IcrdRequestCrdPlay(const Card* pcrdLead, Suit suitTrump) = 0;
	virtual Suit SuitChooseTrump() = 0;

protected:
	Player* _ppl;
};


class BrainlessController : protected Controller
{
	BrainlessController(Player* ppl) : Controller(ppl) {}
	BetChoice BcRequestDesiredBet() override;
	unsigned int IcrdRequestCrdPlay(const Card* pcrdLead, Suit suitTrump) override;
};

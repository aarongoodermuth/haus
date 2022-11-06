#pragma once

#include "View.h"
#include "Bet.h"
#include "Rules.h"
#include "Player.h"

class Controller
{
public:
	Controller(const Player* ppl) : _ppl(ppl) {}
	virtual BetChoice BcRequestDesiredBet() = 0;
	virtual int IcrdRequestCrdPlay(const Card* pcrdLead) = 0;

protected:
	const Player* _ppl;
};


class BrainlessController : protected Controller
{
	BrainlessController(const Player* ppl) : Controller(ppl) {};
	BetChoice BcRequestDesiredBet() override;
	int IcrdRequestCrdPlay(const Card* pcrdLead) override;
};

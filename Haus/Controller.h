#pragma once

#include "View.h"
#include "Bet.h"

class Controller
{
public:
	virtual BetChoice BcRequestDesiredBet() = 0;
	virtual int IcrdRequestCrdPlay() = 0;
};


class BrainlessController : public Controller
{
	BetChoice BcRequestDesiredBet() override { return BetChoice::PASS; }
	int IcrdRequestCrdPlay() override { return 0; }
};

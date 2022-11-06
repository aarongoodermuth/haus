#include "Controller.h"
#include "Player.h"


BetChoice BrainlessController::BcRequestDesiredBet()
{
	return BetChoice::PASS;
}


int BrainlessController::IcrdRequestCrdPlay(const Card* pcrdLead)
{
	int icrdRet = 0;
	while (!Rules::FLegalPlay(_ppl->_veccrdHand[icrdRet], _ppl->_veccrdHand, pcrdLead))
		icrdRet++;
	return icrdRet;
}

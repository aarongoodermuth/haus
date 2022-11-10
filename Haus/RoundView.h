#pragma once
#include "View.h"

#include <vector>
#include "Cards.h"


class RoundView : public View
{
public:
	RoundView(std::vector<Card>* pveccrdPlayed, std::vector<Card>* pveccrdPlayedThisTurn, std::optional<Suit>* poptsuitTrump);
	
	void Show() override;

private:
	std::vector<Card>* _pveccrdPlayed;
	std::vector<Card>* _pveccrdPlayedThisTurn;
	std::optional<Suit>* _poptsuitTrump;
};


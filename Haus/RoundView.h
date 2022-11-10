#pragma once
#include "View.h"

#include <vector>
#include "Cards.h"


class RoundView : public View
{
public:
	RoundView(std::vector<Card>* pveccrdPlayed, std::vector<Card>* pveccrdPlayedThisTurn, Suit* psuitTrump);
	
	void Show(std::optional<Suit> optsuit) override;

private:
	std::vector<Card>* _pveccrdPlayed;
	std::vector<Card>* _pveccrdPlayedThisTurn;
	Suit* _psuitTrump;
};


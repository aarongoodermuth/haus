#pragma once
#include "View.h"

#include <vector>
#include "Cards.h"

using namespace std;

class RoundView : public View
{
public:
	RoundView(vector<Card>* pveccrdPlayed, vector<Card>* pveccrdPlayedThisTurn, Suit* psuitTrump);
	
	void Show(std::optional<Suit> optsuit) override;

private:
	vector<Card>* _pveccrdPlayed;
	vector<Card>* _pveccrdPlayedThisTurn;
	Suit* _psuitTrump;
};


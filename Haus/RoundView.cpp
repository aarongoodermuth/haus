#include "RoundView.h"

#include <iostream>
#include <string>
#include "Cards.h"

#include "Debug.h"

using namespace std;

RoundView::RoundView(vector<Card>* pveccrdPlayed, vector<Card>* pveccrdPlayedThisTurn) : _pveccrdPlayed(pveccrdPlayed), _pveccrdPlayedThisTurn(pveccrdPlayedThisTurn)
{
}

void RoundView::Show()
{
	cout << "ROUND INFORMATION\n" <<
		"=================\n" <<
		"Trump: " << StrFromSuit(_pveccrdPlayedThisTurn->at(0).suit) << "\n" <<
		"Played Cards: ";

	for (int icrd = 0; icrd < _pveccrdPlayedThisTurn->size(); icrd++)
		cout << _pveccrdPlayedThisTurn->at(icrd).StrAscii() << " ";

}

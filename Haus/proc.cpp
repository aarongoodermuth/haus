#include <random>
#include <iostream>
#include <string>

class Player
{
public:
	Player(){}
	virtual ~Player(){}

	int _roundScore;
	int _roundBet;
	int _bumps;
	bool _taken;
	int _score;
	std::string _name;

	void Reset()
	{
		_roundScore = 0;
		_roundBet = 0;
		_taken = false;
	}

	virtual int GetDesiredBet() { return 0; }

	void ForceBet(int betAmount)
	{
		BetCore(betAmount, true);
		_taken = true;
	}

	int BetCore(int betAmount, bool isBetting)
	{
		if (isBetting)
		{
			_roundBet = betAmount;
			_taken = false;
			return betAmount;
		}
		else
		{
			_roundBet = 0;
			_taken = false;
			return 0;
		}
	}

	int Bet(int currentHighest)
	{
		int betDesired = GetDesiredBet();
		return BetCore(betDesired, betDesired > currentHighest);
	}
};


class Computer : Player
{
public:
	Computer(int nameIndex)
	{
		_name = names[nameIndex];
	}

private:
	const static std::string names[4];
	int GetDesiredBet()
	{
		static int retvals[4] = { 0, 5, 6, 12 };
		return retvals[rand() % 4]; //temp
	}
};

const std::string Computer::names[4] = { "Bender", "Karen", "Jarvis", "HAL" };

class User : Player
{
public:
	User()
	{
		_name = "You";
	}

private:
	int GetDesiredBet()
	{
		return 0; //temp
		/*
		int betDesired = 0;

		if (betDesired == 0)
		return 0;

		while (true)
		{
		if (betDesired == 0)
		return 0;

		if (betDesired > currentHighest && IsValidBet())
		{

		}
		}
		}*/
	}
};

class Game
{
public:
	Game()
	{
		int userPos = rand() % 4;
		posplDealer = rand() % 4;
		for (int i = 0; i < 4; i++)
		{
			if (i == userPos)
				pl[i] = (Player*)new User();
			else
				pl[i] = (Player*)new Computer(i);
		}
	}

	~Game()
	{
		for (int i = 0; i < 4; i++)
			delete pl[i];
	}
	
	void Play()
	{
		do
		{ 
			Round();
		} while (WinnerExists());
		DisplayWinner();
	}

private:	
	Player *pl[4];
	int posplDealer;

	bool WinnerExists()
	{
		for (int i = 0; i < 4; i++)
		{
			if(pl[i]->_score > 31)
				return true;
		}
		return false;
	}

	void DisplayWinner()
	{
		int max = 0;
		for (int i = 0; i < 4; i++)
		{
			max = pl[i]->_score > max ? pl[i]->_score : max;
		}

		for (int i = 0; i < 4; i++)
		{
			if (max == pl[i]->_score)
			{
				std::cout << pl[i]->_name << " wins with a score of " << max << std::endl;
			}
		}
	}

	void Bet()
	{
		int curHighestBid = 4;
		int curBet = 0;
		pl[posplDealer % 4]->ForceBet(curHighestBid);
		int i = posplDealer + 1;
		for (int j = 0; j < 4; i++, j++)
		{
			if (curBet = pl[i % 4]->Bet(curHighestBid))
				curHighestBid = curBet;
		}
	}

	void Round()
	{
		for (int i = 0; i < 6; i++)
		{
			Bet();
			PlayTricks();
			Score();
			posplDealer++;
		}
	}

	void PlayTricks()
	{

	}

	void Score()
	{
		for (int i = 0; i < 4; i++)
		{
			if (pl[i]->_roundScore >= pl[i]->_roundBet)
			{
				pl[i]->_score += pl[i]->_roundScore;
			}
			else
			{
				pl[i]->_roundScore -= pl[i]->_roundBet;
				pl[i]->_bumps++;
				if (!pl[i]->_taken)
					pl[i]->_bumps++;
			}

			pl[i]->Reset();
		}
	}

	void DetermineWinner(){}

};

int main()
{
	Game g = Game();
	g.Play();
	return 0;
}
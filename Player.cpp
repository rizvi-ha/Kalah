#include "Player.h"
#include <iostream>
#include <string>
using namespace std;

//Player Implementation

Player::Player(std::string name)
	: m_name(name)
{}

std::string Player::name() const
{
	return m_name;
}

bool Player::isInteractive() const // virtual
{
	return false;
}

Player::~Player() // virtual
{}

//Human Player Implementation

HumanPlayer::HumanPlayer(std::string name)
	: Player(name)
{}

bool HumanPlayer::isInteractive() const
{
	return true;
}

int HumanPlayer::chooseMove(const Board& b, Side s) const
{
	cout << "Choose your move (type a number 1-" << b.holes() << ")" << endl;
	int input = -1;

	while (input < 1 || input > b.holes()) // reprompts until valid
	{
		cin >> input;
		cin.ignore(256, '\n');
		if (b.beans(s, input) == 0)
			input = -1;
	}
		

	return input;
}

//Bad Player Implementation

BadPlayer::BadPlayer(std::string name)
	: Player(name)
{}

int BadPlayer::chooseMove(const Board& b, Side s) const
{
	int leftmost = -1; //Bad Player will just find the leftmost non-empty hole and choose that.

	for (int i = b.holes(); i > 0; i--)
	{
		if (b.beans(s, i) > 0)
		{
			leftmost = i;
		}
	}

	return leftmost;

}

//Smart Player Implementation

SmartPlayer::SmartPlayer(std::string name)
	: Player(name)
{}

int SmartPlayer::chooseMove(const Board& b, Side s) const
{
	int bestHole = -100;
	double value = -1;
	double timeLimit = 19890;  // 11.89 seconds; allow 110 ms for wrapping up
	JumpyTimer timer(1000);

	bestMove(s, b, 1, bestHole, value, timer, timeLimit);

	if ((b.beans(s,bestHole) == 0) || (b.beans(s, bestHole) == -1)) //Catching illegal moves (possibly from errors in bestMove)
	{
		cerr << "oopsie, bestMove gave me a bad answer, time to make a dumb move" << endl;
		for (int i = 1; i <= b.holes(); i++) // basically just does the leftmost hole if bestMove breaks
		{
			if (b.beans(s, i) > 0)
			{
				bestHole = i;
				break;
			}
				
		}
	}

	return bestHole;
}

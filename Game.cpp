#include "Game.h"
#include <iostream>
#include <limits>

Game::Game(const Board& b, Player* south, Player* north)
	: m_board(b), m_south(south), m_north(north), gameOver(false)
{}

void Game::display() const
{
	std::string north_name = m_north->name();
	std::string south_name = m_south->name();
	int north_length = north_name.size();

	//Top Border
	cout << "--------------------------------------------------" << endl;

	//First Line
	for (int i = 0; i < north_length; i++)
	{
		cout << " ";
	}

	cout << "      ";

	for (int i = 1; i <= m_board.holes(); i++)
	{
		cout << m_board.beans(NORTH, i) << "  ";
	}
	cout << endl;

	//Second Line
	cout << north_name;

	if (m_board.beans(NORTH, 0) < 10)
		cout << "   " << m_board.beans(NORTH, POT) << "  ";
	else
		cout << "  " << m_board.beans(NORTH, POT) << "  ";

	for (int i = 1; i <= m_board.holes(); i++)
	{
		cout << "   ";
	}

	cout << m_board.beans(SOUTH, POT) << "  " << south_name;

	cout << endl;

	//Third Line
	for (int i = 0; i < north_length; i++)
	{
		cout << " ";
	}

	cout << "      ";

	for (int i = 1; i <= m_board.holes(); i++)
	{
		cout << m_board.beans(SOUTH, i) << "  ";
	}
	cout << endl;

	//Bottom Border
	cout << "--------------------------------------------------" << endl;
	
}

void Game::status(bool& over, bool& hasWinner, Side& winner) const
{ // Basically just using if and else statements to do what the spec said to do
	if (gameOver)
	{
		over = true;
	}
	else
	{
		over = false;
		return;
	}

	if (m_board.beans(NORTH, POT) == m_board.beans(SOUTH, POT))
	{
		hasWinner = false;
		return;
	}
	else
	{
		hasWinner = true;
	}

	if (m_board.beans(SOUTH, POT) > m_board.beans(NORTH, POT))
		winner = SOUTH;
	else
		winner = NORTH;

}

bool Game::move(Side s)
{	
	Player* m_player;
	Side opp = opponent(s);

	if (s == SOUTH)
		m_player = m_south;
	else
		m_player = m_north;

	bool turnOver = false;

	while (!turnOver) //Using a game loop to allow for the cases where we make multiple turns in one move
	{
		if (m_board.beansInPlay(s) < 1) //Catching game-over scenarios when there is no legal move
		{
			for (int i = 1; i <= m_board.holes(); i++)
			{
				m_board.moveToPot(opp, i, opp);
			}
			gameOver = true;
			return false;
		}

		Side endSide;
		int endHole;
		int move = m_player->chooseMove(m_board, s);
		m_board.sow(s, move, endSide, endHole);

		if (endSide == s && endHole == POT) //Catching double moves (continue causes a second opportunity to move)
		{
			display();
			continue;
		}
		else if ((endSide == s && beans(endSide, endHole) == 1) && beans(opp, endHole) > 0) //Capturing and ending turn when appropriate
		{
			m_board.moveToPot(s, endHole, s);
			m_board.moveToPot(opp, endHole, s);
			display();
			turnOver = true;
		}
		else { //ending turn
			display();
			turnOver = true;
		}
		
	}

	return true;
}

void Game::play()
{
	bool stillPlaying = true;
	bool hasWinner = false;
	Side winner;
	
	display(); //Display initial game state

	while (stillPlaying) //While-loop allows us to repeatedly move south then north
	{

		if (!m_south->isInteractive()) //Buffer for AI player
		{
			cout << "Press Enter to Continue";
			cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
		}
		stillPlaying = move(SOUTH); //move returns a bool so this allows us to see if we are still playing

		if (!stillPlaying) //Makes sure we don't ask north to move if the game is over on South's turn
			break;

		if (!m_north->isInteractive()) //Buffer for AI player
		{
			cout << "Press Enter to Continue";
			cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
		}
		stillPlaying = move(NORTH);

	}

	status(stillPlaying, hasWinner, winner); //Gets the status for us

	display(); // Show final game state
	cout << endl;

	//Display winner or tie game
	if (!hasWinner)
		cout << "Tie Game!" << endl;
	else if (winner == SOUTH)
		cout << m_south->name() << " has won Kalah!" << endl;
	else if (winner == NORTH)
		cout << m_north->name() << " has won Kalah!" << endl;
	else
		cout << "Something has gone wrong" << endl;

}

int Game::beans(Side s, int hole) const
{
	return m_board.beans(s, hole);
}
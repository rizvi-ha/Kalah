#include "Board.h"

Board::Board(int nHoles, int nInitialBeansPerHole)
{
	if (nHoles < 1) //Fixing erroneous inputs
		nHoles = 1;

	if (nInitialBeansPerHole < 0)
		nInitialBeansPerHole = 0;

	//First make the pots
	m_holes[NORTH].push_back(0);
	m_holes[SOUTH].push_back(0);

	for (int i = 0; i < nHoles; i++) //Then fill in the holes
	{
		m_holes[NORTH].push_back(nInitialBeansPerHole);
		m_holes[SOUTH].push_back(nInitialBeansPerHole);
	}
}

int Board::holes() const
{
	return m_holes[NORTH].size() - 1; //Subtracting 1 to not count pot
}

int Board::beans(Side s, int hole) const
{
	if (hole > holes() || hole < 0) //Catching invalid hole indices
		return -1;

	return m_holes[s][hole];
}

int Board::beansInPlay(Side s) const
{
	int count = 0;

	for (int i = 1; i <= holes(); i++) //summing beans on side s
	{
		count += m_holes[s][i];
	}

	return count;
}

int Board::totalBeans() const
{
	int count = 0;

	for (int i = 0; i <= holes(); i++) //summing all beans
	{
		count += m_holes[NORTH][i];
		count += m_holes[SOUTH][i];
	}

	return count;
}

bool Board::sow(Side s, int hole, Side& endSide, int& endHole)
{ //going to put everything into one vector in counter-clockwise direction for ease of iteration

	if ((hole == 0 || beans(s, hole) == 0) || beans(s, hole) == -1) //Catching erroneous input
		return false;

	vector<int*> m_vec;
	int starting_index = -1;

	//Adding all the holes to m_vec, starting with South 1 and ending with North Pot going counter-clockwise

	for (int i = 1; i <= holes(); i++)
	{
		if (s == SOUTH && i == hole) //If the hole we are adding is the hole we are sowing from, make a note of the the corresonding index in m_vec
		{
			m_vec.push_back(&m_holes[SOUTH][i]);
			starting_index = i - 1;
		}
		else
			m_vec.push_back(&m_holes[SOUTH][i]);
	}

	m_vec.push_back(&m_holes[SOUTH][POT]);
	
	if (starting_index == -1) //If our starting point isn't on south side, we use this to help us place the starting index correctly while we add North side holes
	{
		starting_index = holes();
	}

	for (int i = holes(); i >= 0; i--)
	{
		if (s == NORTH && i == hole) //If the hole we are adding is the hole we are sowing from, make a note of the the corresonding index in m_vec
		{
			m_vec.push_back(&m_holes[NORTH][i]);
			starting_index += holes() + 1 - i;
		}
		else
			m_vec.push_back(&m_holes[NORTH][i]);
	}

	//Now that our data is organized how we like, proceed to sow

	int beansToSow = m_holes[s][hole]; //Picking up our beans from the starting point
	m_holes[s][hole] = 0;
	starting_index++; // Making starting index point to the hole AFTER where we just picked up our beans
	int previous_index = 0;
	int* opposing_pot = &m_holes[opponent(s)][POT];

	while (beansToSow > 0)
	{
		if (opposing_pot == m_vec[starting_index])
		{
			starting_index = (starting_index + 1) % m_vec.size();
			continue;
		} //skips loop if opposing pot is about to be placed into
			
		*m_vec[starting_index] += 1;
		beansToSow--;
		previous_index = starting_index;
		starting_index = (starting_index + 1) % m_vec.size(); // this is what allows us to keep going circularly around
	}

	//We have sowed, now time to find where our ending point was in our original array of vectors (m_holes) so we can set endSide and endHole
	int* endpt = m_vec[previous_index];

	for (int i = 0; i <= holes(); i++)
	{
		if (endpt == &m_holes[SOUTH][i])
		{
			endSide = SOUTH;
			endHole = i;
		}
	}

	for (int i = 0; i <= holes(); i++)
	{
		if (endpt == &m_holes[NORTH][i])
		{
			endSide = NORTH;
			endHole = i;
		}
	}

	return true;

}

bool Board::moveToPot(Side s, int hole, Side potOwner)
{
	if ((beans(s, hole) == -1) || hole == 0) // catching invalid hole indices
		return false;

	m_holes[potOwner][POT] += m_holes[s][hole]; //Adding beans to pot then clearing hole(s, hole)
	m_holes[s][hole] = 0;
	return true;
}

bool Board::setBeans(Side s, int hole, int beans)
{
	if (hole > holes() || hole < 0) //Catching invalid hole indices
		return false;
	if (beans < 0) // Catching negative beans
		return false;

	m_holes[s][hole] = beans;
	return true;
}

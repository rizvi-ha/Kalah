# Kalah

Mancala AI

This was a project to code the game Mancala from the ground up in C++ and make an AI that could choose optimal moves within a 10 second time restriction, 
I implemented this using a minimax algorithm and game tree to evaluate future positions and find the best outcome

# Game Implementation:

My implementation of Game was pretty straightforward, here are the private data members I used:  
  
private:  
	Player* m_south;  
	Player* m_north;  
	Board m_board;  
	bool gameOver;  
  
Basically had a pointer to our two players, a board, and a boolean telling us when the game was over. The gameOver bool would be set to true when move(...) was called and there was no legal move to make. This gameOver bool was then used in the status(...) function to determine if the game was over.

Code-wise not anything super fascinating here, used a while loop for the move(...) function that would end when the turn is over, allowing there to be multiple turns in a row when needed (like when your beans land in your own pot).

# Player Implementation:

Very simple implementation, was careful to construct things properly and everything worked out. HumanPlayer overrides the virtual isInteractive function to return true. HumanPlayer asks the player to type their move and reprompts if it is invalid.

BadPlayer just finds the leftmost non-empty hole and returns that for chooseMove

smartPlayer::chooseMove itself is pretty simple, it uses non-member function bestMove (which in turns calls non-member eval(...)) and gives it 9.89 seconds to do its thing. Then it double-checks that bestMove gave a valid move (makes a dumb move if not) and returns the move that bestMove chose.

The complexity lies in bestMove and eval, which I will run through and give pseudocode of below


## void bestMove(Side s, const Board& b, int depth, int &bestHole, double &value, JumpyTimer& timer, double timeLimit)

bestMove takes in which side is moving, the Board being played on, current search depth, and returns the bestHole, the value (given by eval) of the board state caused by that best move.

You’ll notice the last two inputs, timer and timeLimit correspond to the methods for keeping bestMove under 10 seconds, while still searching all branches of the tree equally. Below is the pseudocode

void bestMove(in: side, board, timer, timeLimit. Out: bestHole, value)
	Current board state = eval(board)

	If theres no legal move
		bestHole = -1
		Value = current board state
		Return
	If reached depth or time limit
		bestHole = -1
		Value = current board state
		Return

	For all the holes on our side that are not empty (legal moves)
		Within this branches allotted time limit
		“Make” the move on a copy of the board
		If we land in our own pot
			bestMove(side, boardcopy, depth, h2, v2, timer, thisBranchTimeLimit);
		If a capture should happen
			Update the board to reflect the capture
			bestMove with depth +1 and opponent as the side
		Else
			bestMove with depth +1 and opponent as the side

		If v2 is better for us then we’ve seen so far
			bestHole = h2
			Value = v2

	return

## double eval(const Board& b)

eval takes in a board and returns a value from 0 to 1. A value of 0 represents North’s win and a value of 1 represents South’s win. 

Basically the way it works is that if it sees that one of the sides has over totalBeans/2 beans in their pot, they have won so it returns without doing any calculations

When that’s not the case, it does calculations to get that score from 0 to 1.

Theres two heuristics that I used, one is the difference between the two pots, and the other is how many of the total beans are on south’s side.

I then scaled these two values to range from 0 to 1, and then combined them. At first I was dynamically scaling the weight of each score based on how close the game was to being over, but in the end what ended up working the best was having the score be 93% the difference between the two pots and 7% how many beans are on south’s side. This way it mostly looks to the much more important metric but can use hoarding beans as a tie-breaker when deciding moves.

# Board Implementation:

My only private data member was an array of vectors, declared as such:

vector<int> m_holes[2];

This worked out nicely because using m_holes[SOUTH] would give me the vector containing south’s holes and m_holes[NORTH] would give me the vector containing north’s holes.

I had the vectors organized so that m_holes[SOUTH][POT] (the 0th entry in the vector) would store the pot’s beans. Then the vector indices 1-whatever held the beans corresponding to that hole number. This led to a very very simple implementation of most everything, but a fairly complicated implementation of Board::sow.

The way I did Board::sow was that inside the function I made a vector<int*> m_vec that would store pointers to the holes we needed to modify. I then used for loops to add the all the holes starting with South 1 and ending with North Pot going counter-clockwise. Having everything in one vector in counter-clockwise order then made sowing a lot easier. 

I then compared the pointer in m_vec of wherever the sow ended to all of the memory locations of everything in m_holes to find endHole and endSide.

All in all it ended up working pretty seamlessly.


# Bugs/Issues

Actually while writing this report I realized that in my bestMove() function I was incorrectly having the same side go again after a capture, instead of having the opponent go as the game actually does. This explains why one of my friends who was really good at Mancala was able to beat my AI, because until now my AI was foolishly thinking it could go again after a capture when calculating its best move. I have now fixed the issue.

Also my display() function can be kinda derpy when there are two digit numbers, the spacing can get a little bit off but its nothing too bad. 

Other than that, I actually didn’t have too much bugs throughout development, I followed the advice section of my professor and built everything very incrementally and it seems to have worked out.

# Test Cases

I tested SmartPlayer being smart by playing against it and having friends play against it. 

I tested Game working properly mostly by playing games, rather than using as much assert statements.

Everything else I tested using an extensive list of my own assert statements

Here are some of my own assert statements I used while developing:

    //Testing Board.cpp
    Board b = Board(4, 2);
    Board c = Board(0, -2);
    assert(b.holes() == 4);
    assert(c.holes() == 1);
    assert(b.beans(NORTH, -1) == -1);
    assert(b.beans(NORTH, 0) == 0);
    assert(b.beans(SOUTH, 1) == 2);
    assert(b.beans(NORTH, 2) == 2);
    assert(b.beans(NORTH, 5) == -1);
    assert(b.beans(SOUTH, 4) == 2);
    assert(c.beans(SOUTH, -1) == -1);
    assert(c.beans(SOUTH, 1) == 0);
    assert(c.beans(NORTH, 0) == 0);
    assert(c.beans(NORTH, 2) == -1);
    assert(b.beansInPlay(NORTH) == 8);
    assert(b.beansInPlay(SOUTH) == 8);
    assert(c.beansInPlay(SOUTH) == 0);
    assert(b.totalBeans() == 16);
    assert(c.totalBeans() == 0);
    Side s = NORTH;
    int h = 0;
    assert(b.sow(s, 1, s, h));
    assert(s == SOUTH);
    assert(h == 1);
    assert(b.sow(s, 0, s, h) == false);
    assert(s == SOUTH);
    assert(h == 1);
    assert(b.setBeans(NORTH, 5, 2) == false);
    assert(b.setBeans(NORTH, 4, -1) == false);
    assert(b.setBeans(SOUTH, 4, 6) == true);
    assert(b.sow(s, 4, s, h));
    assert(s == SOUTH);
    assert(h == 1);
    assert(b.setBeans(s, h, 10));
    assert(b.sow(s, h, s, h));
    assert(s == SOUTH);
    assert(h == 2);
    assert(b.beansInPlay(SOUTH) == 9);
    assert(b.beansInPlay(NORTH) == 14);
    assert(b.totalBeans() == 26);
    assert(b.moveToPot(SOUTH, 2, NORTH));
    assert(b.beans(SOUTH, 2) == 0);
    assert(b.beans(NORTH, 0) == 5);
    assert(b.beansInPlay(NORTH) == 14);
    assert(b.beansInPlay(SOUTH) == 5);
    assert(b.totalBeans() == 26);

    //Testing Player.cpp
    BadPlayer badp = BadPlayer("Jon");
    assert(badp.name() == "Jon");
    assert(badp.chooseMove(b, NORTH) == 1);
    
    //Testing Board Copy Constructor and Operator=
    Board bb = b;
    c = bb;
    assert(bb.beans(SOUTH, 2) == 0);
    assert(bb.beans(NORTH, 0) == 5);
    assert(bb.beansInPlay(NORTH) == 14);
    assert(bb.beansInPlay(SOUTH) == 5);
    assert(bb.totalBeans() == 26);
    assert(c.beans(SOUTH, 2) == 0);
    assert(c.beans(NORTH, 0) == 5);
    assert(c.beansInPlay(NORTH) == 14);
    assert(c.beansInPlay(SOUTH) == 5);
    assert(c.totalBeans() == 26);
    assert(b.sow(s, 4, s, h));
    assert(b.sow(s, 1, s, h));
    assert(b.sow(s, 2, s, h));
    assert(bb.beans(SOUTH, 2) == 0);
    assert(bb.beans(NORTH, 0) == 5);
    assert(bb.beansInPlay(NORTH) == 14);
    assert(bb.beansInPlay(SOUTH) == 5);
    assert(bb.totalBeans() == 26);
    assert(c.beans(SOUTH, 2) == 0);
    assert(c.beans(NORTH, 0) == 5);
    assert(c.beansInPlay(NORTH) == 14);
    assert(c.beansInPlay(SOUTH) == 5);
    assert(c.totalBeans() == 26);
 


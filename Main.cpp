
#if defined(_MSC_VER)  &&  !defined(_DEBUG)
#include <iostream>
#include <windows.h>
#include <conio.h>

struct KeepWindowOpenUntilDismissed
{
    ~KeepWindowOpenUntilDismissed()
    {
        DWORD pids[1];
        if (GetConsoleProcessList(pids, 1) == 1)
        {
            std::cout << "Press any key to close this window . . . ";
            _getch();
        }
    }
} keepWindowOpenUntilDismissed;
#endif

#include <iostream>
#include <cassert>
#include <chrono>
#include "Board.h"
#include "Player.h"
#include "Game.h"
using namespace std;

double eval(const Board& b) // going to return a score between 0 and 1; 0 being north won and 1 being south won
{
    Side s = SOUTH;
    Side opp = opponent(s);
    double alpha; //score-scaling constant
    double beta; //score-scaling constant

    //Note: Below I am using the fact that once a side has more than half of the beans in their pot, its impossible for the enemy to win

    if (b.beans(s, POT) > b.totalBeans() / 2) // If South wins this position return 1 without doing any costly computations
    {
        return 1;
    }
    else if (b.beans(opp, POT) > b.totalBeans() / 2) // If North wins this position return 1 without doing any costly computations
    {
        return 0;
    }

    double score1; //How many beans are on my side? 
    double score1_scaled;
    int totalBeansInPlay = b.beansInPlay(s) + b.beansInPlay(opp);
    score1 = totalBeansInPlay - b.beansInPlay(s);

    score1_scaled = 1 - (score1 / totalBeansInPlay); //Scaling to range from 0 to 1

    double score2; //Difference between my pot and opponent pot
    double score2_scaled;
    score2 = b.beans(s, POT) - b.beans(opp, POT);

    score2_scaled = (score2 + b.totalBeans()) / (2 * (b.totalBeans())); //Scaling to range from 0 to 1

    //I want score 2 to be more important when the amount in the leading pot approaches b.totalBeans()/2
    //I will use a third score, score 3 to do this
    /*double score3;
    double score3_scaled;
    double half = (b.totalBeans() / 2);
    int leadingPot = max(b.beans(s, POT), b.beans(opp, POT));
    score3 = half - leadingPot;
    score3_scaled = -1 * ((score3 - half) / (half));*/

    beta = 0.93; //Having the evaluation use mostly score2 leads to better results than scaling it dynamically
    alpha = 1 - beta; // Coefficients add to 1 so that our result still is scaled from 0 to 1

    double eval = (alpha * score1_scaled) + (beta * score2_scaled);

    if (eval > 1 || eval < 0) // Catching errors
            cerr << "oops";

    return eval;
    
}

void bestMove(Side s, const Board& b, int depth, int &bestHole, double &value, JumpyTimer& timer, double timeLimit)
{
    //Checking if there is a legal move
    bool thereIsALegalMove = false;
    for (int i = b.holes(); i > 0; i--)
    {
        if (b.beans(s, i) > 0)
        {
            thereIsALegalMove = true;
        }
    }

    double state = eval(b); //Only calculating eval(b) once and storing it to save on time

    //Base cases where we stop recursively calling
    if (!thereIsALegalMove)
    {
        bestHole = -1;
        value = state;
        return;
    }
    else if (timeLimit <= 0)
    {
        bestHole = -1;
        value = state;
        return;
    }
    else if (depth > 10)
    {
        bestHole = -1;
        value = state;
        return;
    }

    double bestValue = -1; //Keeping track of the best board state we've seen so far

    for (int i = 1; i <= b.holes(); i++) //Looping through possible moves
    {
        double thisBranchTimeLimit = timeLimit / ((b.holes() + 1) - i); //Giving each branch a fraction of the total time as suggested in Project 3 FAQ
        double startTime = timer.elapsed();

        if (b.beans(s, i) == 0) //If this isn't a legal move don't need to consider it
            continue;

        Board m_board = b;
        
        Side endSide;
        int endHole;
        int h2 = 0;
        double v2 = 0;

        m_board.sow(s, i, endSide, endHole); //"Make" the move on our copy of the board

        if (endSide == s && endHole == POT) //If we land in our pot same side goes again
        {
            bestMove(s, m_board, depth, h2, v2, timer, thisBranchTimeLimit);
            timeLimit -= (timer.elapsed() - startTime);
        }
        else if ((endSide == s && m_board.beans(endSide, endHole) == 1) && m_board.beans(opponent(endSide), endHole) > 0) //If we capture need to update our copy of the board
        {
            m_board.moveToPot(s, endHole, s);
            m_board.moveToPot(opponent(s), endHole, s);
            bestMove(opponent(s), m_board, depth + 1, h2, v2, timer, thisBranchTimeLimit);
            timeLimit -= (timer.elapsed() - startTime);
        }
        else { //Opponent makes their best move
            bestMove(opponent(s), m_board, depth + 1, h2, v2, timer, thisBranchTimeLimit);
            timeLimit -= (timer.elapsed() - startTime);
        }

        //If the resulting board state is better for us than we've ever seen before then this is our bestHole so far
        if ((v2 >= bestValue) && (s == SOUTH))
        {
            bestValue = v2;
            bestHole = i;
            value = v2;
        }
        else if ((1 - v2 >= bestValue) && (s == NORTH))
        {
            bestValue = 1 - v2; 
            bestHole = i;
            value = v2;
        }
        
    }

    return;

}

int main()
{
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
 

    

    Board d(6, 4);
    HumanPlayer j("South");
    SmartPlayer bar("North");
    Game gme = Game(d, &j, &bar);
    gme.play();
    
    

    cerr << "yipee" << endl;
}
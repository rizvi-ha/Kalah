#ifndef SIDE_INCLUDED
#define SIDE_INCLUDED

#include <chrono>

enum Side { NORTH, SOUTH };

const int NSIDES = 2;
const int POT = 0;

class Board;

class Timer //Timer class from the Project 3 FAQ (didn't need any adapting)
{
public:
    Timer()
    {
        start();
    }
    void start()
    {
        m_time = std::chrono::high_resolution_clock::now();
    }
    double elapsed() const
    {
        std::chrono::duration<double, std::milli> diff =
            std::chrono::high_resolution_clock::now() - m_time;
        return diff.count();
    }
private:
    std::chrono::high_resolution_clock::time_point m_time;
};

class JumpyTimer //JumpyTimer from the Project 3 FAQ (didn't need any adapting)
{
public:
    JumpyTimer(int jumpInterval)
        : m_jumpInterval(jumpInterval), m_callsMade(0)
    {
        actualElapsed();
    }
    double elapsed()
    {
        m_callsMade++;
        if (m_callsMade == m_jumpInterval)
        {
            m_lastElapsed = m_timer.elapsed();
            m_callsMade = 0;
        }
        return m_lastElapsed;
    }
    double actualElapsed()
    {
        m_lastElapsed = m_timer.elapsed();
        return m_lastElapsed;
    }
private:
    Timer m_timer;
    int m_jumpInterval;
    int m_callsMade;
    int m_lastElapsed;
};


inline
Side opponent(Side s)
{
	return Side(NSIDES - 1 - s);
}

double eval(const Board& b); //Returns how good a position is for South

void bestMove(Side s, const Board& b, int depth, int &bestHole, double &value, JumpyTimer& timer, double timeLimit); 
//Finds the best move for side s on board b and sets bestHole to that move (and does it in <5 seconds)


#endif
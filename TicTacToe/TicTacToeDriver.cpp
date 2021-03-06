#include "stdafx.h"
#include "iostream"
#include <list>
#include <random>
#include <bitset>
#include <time.h>
using namespace std;

bool alpha_beta_on;

void print_directions()
{
	std::cout << "This 3x3 TicTacToe game allows you to play\n";
	std::cout << "against another human player or against the machine.\n";
	std::cout << "You will be propted for these choices.\n";
	std::cout << "Should you choose to play againts a machine\n";
	std::cout << "you will be prompted whether you want the machine to go first.\n";
	std::cout << "Also, you will be prompted whether alpha-beta should be used.\n\n";
	std::cout << "The TicTacToe will be indexed starting at 0 through 8 in row major form.\n";
	std::cout << "0 1 2\n3 4 5\n6 7 8\n";
	std::cout << "A human player will be prompted for the square number between 0-8.\n";
	std::cout << "A human player will be prompted again if they enter a square number out of range\n";
	std::cout << "or already filled\n";
}
bool optionPrompt(const char *p)
{
	char option;
	std::cout << p << "? ";
	cin >> option;
	if (option == 'Y' || option == 'y') {
		return true;
	}
	else {
		return false;
	}
}

/*** INSTRUCTIONS ***/
//How to represent a gameState?
// Squares are represented with the following indices
/*
0 1 2
3 4 5
6 7 8

Could represent with and X "board" and a separate O "board"
and use bitstrings for each board: 1 if a square is occupied, 0 if not.

layout bits
8 7 6 5 4 3 2 1 0

| X |
X | X | O
| O |
Would be represented as 8 7 6 5 4 3 2 1 0
gameState.x: 0 0 0 0 1 1 0 1 0
gameState.o: 0 1 0 1 0 0 0 0 0
.x | .o   =  0 1 0 1 1 1 0 1 0
1 represented as
0 0 0 0 0 0 0 ... 0 0 0 0 1
1 << 2
0 0 0 0 ...0 0 0 0 1 0 0

((.x | .o)  & (1 << 2)) =   0 1 0 1 1 1 0 1 0
&    0 0 0 0 0 0 1 0 0
0 0 0 0 0 0 0 0 0

!((.x | .o)  & (1 << 1))  is 0
O | X | O
X | X | O
X | O | X
Would be represented as 8 7 6 5 4 3 2 1 0
gameState.x: 1 0 1 0 1 1 0 1 0
gameState.o: 0 1 0 1 0 0 1 0 1
.x | .o   =  1 1 1 1 1 1 1 1 1
0x1FF


*/
class action
{
public:
	action()
	{
		a = 0;
		side = 0;
	}
	action(short aParm, short sideParm)
	{
		a = aParm;
		side = sideParm;
	}
	short a;
	short side;
	enum Player { MIN = 1, MAX = 2 };
	/* side is for who is making the mark. If side is set to action::MIN, then
	the action a will be applied to gamestate.o
	If side is set to action::MAX, then action a will applied to gamestate.x */
private:



};
typedef int utility;

class gameState
{
public:
	short x;
	short o;

	gameState()
	{  //blank tictactoe grid
		x = 0;
		o = 0;
	}

	bool isFilled() const
	{
		return ((x | o) == 0x1FF);
	}
	bool TOP_ROW(short z) const //z is either x or o 
	{
		return (z & 7) == 7; //Comparing to binary value
	}
	bool MID_ROW(short z) const
	{
		return (z & 56) == 56;
	}
	bool BOT_ROW(short z) const
	{
		return (z & 448) == 448;
	}
	bool LEFT_COL(short z) const
	{
		return (z & 73) == 73;
	}
	bool MID_COL(short z) const
	{
		return (z & 146) == 146;
	}
	bool RIGHT_COL(short z) const
	{
		return (z & 292) == 292;
	}
	bool DiagNW_SE(short z) const
	{
		return (z & 273) == 273;
	}
	bool DiagNE_SW(short z) const
	{
		return (z & 84) == 84;
	}
	bool win(short s) const {
		return
			TOP_ROW(s) ||
			MID_ROW(s) ||
			BOT_ROW(s) ||
			LEFT_COL(s) ||
			MID_COL(s) ||
			RIGHT_COL(s) ||
			DiagNW_SE(s) ||
			DiagNE_SW(s);
	}
	bool minWin() const {
		return win(o);
	}
	bool maxWin() const {
		return win(x);
	}
};

ostream & operator<<(ostream & out, gameState & gs)
{
	std::bitset<9> x(gs.x);
	std::bitset<9> o(gs.o);
	for (int i = 0; i <= 8; i++)
	{
		if (x[i] == 1) // if occupied print x
			out << "x ";
		else if (o[i] == 1) // if occupied print o
			out << "o ";
		else // otherwise print _
			out << "_ ";
		if (i % 3 == 2) // move to next line
			out << endl;
	}
	return out;
}

short getValidChoice(const gameState &gs, const char *p)
{
	//Check index for valid range 0-8
	//check for unoccupied square
	short index;
	while (true)
	{
		std::cout << p;
		cin >> index;
		if (cin.good())
		{
			if (index >= 0 && (index <= 8))
				if (!((gs.x | gs.o) & (1 << index)))
					break;
		}
		else
		{
			cin.clear();
			cin.ignore(INT_MAX, '\n');
		}
		std::cout << "Invalid input, expected [0-8]" << endl;
	}
	return index;
}

gameState applyCandidate(const gameState &gs, const action &act)
{
	gameState next_gs;
	short move = 0;
	move |= (1 << (act.a));
	if (act.side == action::MIN)
	{  //O moves
		next_gs.x = gs.x;
		next_gs.o = gs.o | move;
	}
	else if (act.side == action::MAX)
	{	//X moves
		next_gs.x = gs.x | move;
		next_gs.o = gs.o;
	}
	return next_gs;
}

bool endState(const gameState& gs) //Check for game over
{
	return gs.maxWin() || gs.minWin() || gs.isFilled();
}
void eval(const gameState& gs, int& val) //Check for winner
{
	if (gs.maxWin())
		val = 1;
	else if (gs.minWin())
		val = -1;
	else
		val = 0;
}

void getCandidates(const gameState& gs, list<action>& candidates, action::Player player)
{
	short open = gs.o | gs.x;
	std::bitset<9> op(open);
	for (int i = 0; i < 9; i++)
	{
		if (op[i] == 0)
			candidates.push_back(action(i, player));
	}
}

void minValue(const gameState&, utility, utility, action&, utility&);

void maxValue(const gameState& gs, utility alpha, utility beta, action& act, utility& val)
{
	utility bestUtility = -INT_MAX;  //v in the textbook pseudo-code
	action bestAction;

	if (endState(gs))
	{
		eval(gs, val); //eval just assigns -1, 0 , +1
		return;
	}
	list<action> candidates;
	getCandidates(gs, candidates, action::MAX);

	//iterate over the candidates. 
	list<action>::iterator itr;
	list<action>::iterator end = candidates.end();
	for (itr = candidates.begin(); itr != end; itr++)
	{
		gameState next_gs = applyCandidate(gs, *itr);
		utility next_utility;
		action next_action;

		minValue(next_gs, alpha, beta, next_action, next_utility);
		if (next_utility > bestUtility)
		{
			bestUtility = next_utility;
			bestAction = *itr;
		}
		if (alpha_beta_on)
		{
			if (next_utility >= beta)
			{
				break; //stop looking for candidates. 
			}
			else
			{
				alpha = (next_utility > alpha) ? next_utility : alpha;
			}
		}
	}
	act = bestAction;
	val = bestUtility;
}

void minValue(const gameState& gs, utility alpha, utility beta, action& act, utility& val)
{
	utility bestUtility = INT_MAX;  //v in the textbook pseudo-code
	action bestAction;

	if (endState(gs))
	{
		eval(gs, val); //eval just assigns -1, 0 , +1
		return;
	}
	list<action> candidates;
	getCandidates(gs, candidates, action::MIN);

	//iterate over the candidates. 
	list<action>::iterator itr;
	list<action>::iterator end = candidates.end();
	for (itr = candidates.begin(); itr != end; itr++)
	{
		gameState next_gs = applyCandidate(gs, *itr);
		utility next_utility;
		action next_action;

		maxValue(next_gs, alpha, beta, next_action, next_utility);
		if (next_utility < bestUtility)
		{
			bestUtility = next_utility;
			bestAction = *itr;
		}
		if (alpha_beta_on)
		{
			if (next_utility <= alpha)
				break; //stop looking for candidates. 
			else
			{
				beta = (next_utility < beta) ? next_utility : beta;
			}
		}
	}
	act = bestAction;
	val = bestUtility;
}

void minimax(const gameState& gs, action& act, utility& val)
{
	utility alpha = -INT_MAX;
	utility beta = INT_MAX;
	maxValue(gs, alpha, beta, act, val);
	return;
}

int main()
{
	bool twoHumans;
	bool machineFirst;
	bool done = false;
	short choice;
	print_directions();
	gameState gs;
	srand(time(NULL));
	twoHumans = optionPrompt("Two humans (y/n)");
	if (!twoHumans) machineFirst = optionPrompt("Machine first (y/n)");
	if (twoHumans)
	{ //start alternating between X and O starting with X
		while (!done)
		{
			choice = getValidChoice(gs, "Player X choice? ");
			gs = applyCandidate(gs, action(choice, action::MAX));
			cout << gs << endl;
			if (gs.maxWin())
			{
				cout << "Player X wins!" << endl;
				done = true;
			}
			else if (gs.isFilled())
			{
				cout << "Tie: Cat's game!" << endl;
				done = true;
			}
			//Player O moves
			if (!done)
			{
				choice = getValidChoice(gs, "Player O choice? ");
				gs = applyCandidate(gs, action(choice, action::MIN));
				cout << gs << endl;
				if (gs.minWin())
				{
					cout << "Player O wins!" << endl;
					done = true;
				}
				else if (gs.isFilled())
				{
					cout << "Tie: Cat's game!" << endl;
					done = true;
				}
			}
		}
	}  //end two human-play
	else
	{  //machine vs. human
		alpha_beta_on = optionPrompt("Alpha-beta pruning on (y/n)");
		bool firstMove = true;
		utility val;
		action act;
		if (machineFirst)
		{
			while (true)
			{
				if (firstMove)
				{
					firstMove = false;
					act = action((short)rand() % 8, action::MAX);   //act is an action
					val = 0; // val is a utility value 
				}
				else //see what MAX wants to do
				{
					minimax(gs, act, val);
				}
				gs = applyCandidate(gs, act);

				cout << gs << endl;
				if (gs.maxWin())
				{
					cout << "Machine wins!" << endl;
					break;
				}
				if (gs.isFilled())
				{
					cout << "Tie: Cat's game!" << endl;
					break;
				}
				short o;
				o = getValidChoice(gs, "Square? ");
				gs = applyCandidate(gs, action(o, action::MIN));

				cout << gs << endl;
				if (gs.minWin())
				{
					cout << "Player wins!" << endl;
					break;
				}
				if (gs.isFilled())
				{
					cout << "Tie: Cat's game!" << endl;
					break;
				}
			}
		}
		else // machine second
		{
			while (true)
			{
				action act;
				short o;
				o = getValidChoice(gs, "Square? ");
				gs = applyCandidate(gs, action(o, action::MIN));

				cout << gs << endl;
				if (gs.minWin())
				{
					cout << "Player wins!" << endl;
					break;
				}
				if (gs.isFilled())
				{
					cout << "Tie: Cat's game!" << endl;
					break;
				}

				//See what Max wants to do
				minimax(gs, act, val);

				gs = applyCandidate(gs, act);

				cout << gs << endl;
				if (gs.maxWin())
				{
					cout << "Machine wins!" << endl;
					break;
				}
				if (gs.isFilled())
				{
					cout << "Tie: Cat's game!" << endl;
					break;
				}
			}
		}
	}
	system("pause");
}
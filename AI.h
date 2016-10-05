#ifndef AI_H
#define AI_H
#include <iostream>
#include <string>
#include <stack>

#include "Board.h"

using namespace std;

// Conventions:
/*
Computer is always black. 
*/

const unsigned int targetDepth = 1;

class AI
{
public:
	AI(string FEN);
	void printDebug(string show);
	void printBoard();

	void negaMax_Search();
	void debug();
	//void search_evaluate(int targetDepth);
	struct Node{
		vector<u32> moves;
		int movePtr;
		float alpha, beta;
	};
private:
	Board board;
};

#endif
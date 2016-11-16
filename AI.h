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
friend class Benchmark;
public:
	struct Node{
		vector<Move> moves;
		int movePtr;
		float alpha, beta, boardScore;
	};
	AI(string FEN);
	void printDebug(string show = "prnbkqPRNBKQ");
	void printBoard();

	/*~~~~~~~~~~~ Master function ~~~~~~~~~~~*/
	void negaMax_Search(Node* node, int depth);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	void debug();
	const Board& getBoardRef();
private:
	Board chessBoard;
	color sideToMove;
};

#endif

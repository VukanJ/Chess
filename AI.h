#ifndef AI_H
#define AI_H
#include <iostream>
#include <string>
#include <memory>
#include <stack>

#include "Board.h"

using namespace std;

// Conventions:
/*
Computer is always maximizing player.
*/

const unsigned int targetDepth = 1;

class AI
{
friend class Benchmark;
private:
	Board chessBoard;
	color sideToMove;

	enum moveOrdering{
		QUIET,
		CAPTURE,
		PV_MOVE,
		ALPHABETA_CUTOFF,
		MATE
	};
	class Node
	{
	public:
		Node();
		Move move;
		float boardValue, alpha, beta;
		byte ordering;
		vector<unique_ptr<AI::Node*>> nodes;
	};
public:
	AI(string FEN);
	void printDebug(string show = "prnbkqPRNBKQ");
	void printBoard();

	/*~~~~~~~~~~~ Master function ~~~~~~~~~~~*/
	void negaMax_Search(unique_ptr<AI::Node*> node, int depth);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	void debug();
	const Board& getBoardRef();
};

#endif

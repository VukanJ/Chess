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
friend class UnitTest;
class Node;
typedef unique_ptr<AI::Node> nodePtr;
private:
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
		float boardValue, alpha, beta;
		byte ordering;
		vector<Move> moves;
		vector<nodePtr> nodes;
	};

	/*~~~~~~~~~~~~~~~~~~~ Master function ~~~~~~~~~~~~~~~~~~~*/
	void negaMax_Search(nodePtr& node, int depth, color side);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	nodePtr Root;
	Board chessBoard;
	color sideToMove;
	int targetDepth; // Needed for iterative deepening
public:
	AI(string FEN, color computerColor);
	void printDebug(string show = "prnbkqPRNBKQ");
	void printBoard();

	void Play(); // Play
	
	void debug();
	const Board& getBoardRef();

	// Piece color of computer
	const color aiColor;
};

#endif

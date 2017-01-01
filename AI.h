#ifndef AI_H
#define AI_H
#include <iostream>
#include <string>
#include <algorithm>
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
friend class Gui;
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
		vector<Move> moveList;
		vector<nodePtr> nodeList;
	};
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Master function ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	int negaMax_Search(nodePtr& node, int alpha, int beta, int depth, color side);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	// TODO: Implement NegaScout instead of negaMax

	nodePtr Root;
	Board chessBoard;
	color sideToMove;
	int targetDepth; // Needed for iterative deepening

	ZobristHash transposition_hash;
public:
	AI(string FEN, color computerColor);
	void printDebug(string show = "prnbkqPRNBKQ");
	void printBoard();

	void Play(); // Play
	
	void debug();
	const Board& getBoardRef();
	Board* getBoardPtr();

	// Piece color of computer
	const color aiColor;
};

#endif

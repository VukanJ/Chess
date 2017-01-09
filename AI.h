#ifndef AI_H
#define AI_H
#include <iostream>
#include <cstdio>
#include <string>
#include <algorithm>
#include <memory>
#include <stack>

#include "Board.h"
#include "gui.h"

using namespace std;

// Conventions:
/*
Computer is always maximizing player.
*/

constexpr unsigned int MAX_DEPTH = 64;

class AI
{
friend class Benchmark;
friend class UnitTest;
friend class Gui;
class Node;

typedef unique_ptr<AI::Node> nodePtr;
private:
	enum moveOrdering{
		// Currently unused
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
		byte nodeFlags;
		enum Flags {
			explored = 0x1,
			leaf     = 0x2,
			terminal = 0x4,
			cutNode  = 0x8
		};
	};
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Master functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	pair<Move, int> distributeNegaMax();
	int NegaMax_Search(nodePtr& node, int alpha, int beta, int depth, color side);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	
	// TODO: Implement NegaScout instead of negaMax

	void sortMoves(nodePtr&, color side);

	nodePtr Root;
	Board chessBoard;
	color sideToMove;
	int targetDepth; // Needed for iterative deepening

	ZobristHash transposition_hash;
	Gui* gui;
	bool guiActive;
public:
	AI(string FEN, color computerColor);
	void printDebug(string show = "prnbkqPRNBKQ");
	void printBoard();
	void bindGui(Gui* gui);

	void Play(sf::RenderWindow& window); // Play
	
	void debug();
	const Board& getBoardRef();

	Board* getBoardPtr();

	// Piece color of computer
	const color aiColor;
};

#endif

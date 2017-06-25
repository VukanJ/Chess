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

constexpr uint MAX_DEPTH = 64;

class Gui;

class AI
{
friend class Benchmark;
friend class UnitTest;
friend class Gui;
friend class DataBaseTest;


	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Master functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
	//pair<Move, int> distributeNegaMax();
	//int NegaMax_Search(nodePtr& node, int alpha, int beta, int depth, color side);
	/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

	//void sortMoves(nodePtr&, color side);

	Board chessBoard;
	color sideToMove;
	int targetDepth; // Needed for iterative deepening

	ZobristHash transposition_hash;
	Gui* gui;

	// Gamehistory contains string board representation for
	// exact comparison. (Hash can be overwritten)
	string boardToString() const;
	vector<pair<string, Move>> gameHistory;
public:
	AI(string FEN, color computerColor);
	AI(string FEN, color computerColor, uint hashSize);
	void printDebug(string show = "prnbkqPRNBKQ");
	void printBoard();
	void bindGui(Gui* gui);

	void Play(sf::RenderWindow& window); // Play

	const Board& getBoardRef();

	Board* getBoardPtr();

	void writeToHistory(const Move& move);
	// Piece color of computer
	const color aiColor;

	void resetGame();
};

#endif

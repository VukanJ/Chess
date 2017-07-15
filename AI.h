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
friend class DataBaseTest;
friend class Gui;
private:

	int targetDepth; // Needed for iterative deepening

	Board board;
	ZobristHash transpositionHash;
	PVTable pvTable;
	Gui* gui;

	// Gamehistory contains string board representation for
	// exact comparison. (Hash can be overwritten)
	//string boardToString() const;
	//vector<pair<string, Move>> gameHistory;
	// Chess playing functions
	int NegaMax(int alpha, int beta, int depth, int ply, color side);
	int QuiescenceSearch(int alpha, int beta, int ply, color side);
	void extractPrincipalVariation(const U64& startKey, vector<Move>& pvLine,  int maxPrintDepth, color side);

public:
	AI(string FEN, color computerColor);
	AI(string FEN, color computerColor, uint hashSize);
	void setFen(string);
	pair<Move, Move> getBestMove(color forPlayer, int maxDepth);
	void playStringMoves(const vector<string>& moves, color side);
	void reset();

	void printDebug(string show = "prnbkqPRNBKQ");
	void bindGui(Gui* gui);
	void printBoard();
	void printAscii();

	void writeToHistory(const Move& move);
	// Piece color of computer
	color aiColor;
	color sideToMove;
};

#endif

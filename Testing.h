#ifndef TESTING_H
#define TESTING_H
#include <iostream>
#include <algorithm>
#include <fstream>
#include <random>
#include <memory>
#include <stack>
#include <climits>
#include <string>
#include <chrono>

#include "misc.h"
#include "Move.h"
#include "ZobristHash.h"
#include "Data.h"
#include "defines.h"
#include "AI.h"
#include "Board.h"

using namespace std;

typedef pair<string, string> stringpair;

class ChessError
{
public:
	ChessError(string msg);
	virtual void what() const;
protected:
	string errMsg;
	string fname;
};

class IntrinError : public ChessError
{
public:
	IntrinError(string msg, string fname);
	void what() const override;
private:
	string fname;
};

class UnitTest
{
public:
	UnitTest();

	// Intrinsics testing
	void testDefines() const;
	void testIntrinsics() const;

	// Move generation testing
	void testGenerationAlgorithms();

	// Tree testing
	class MinimalTree { // Low memory Tree, only stores current path
	public:
		MinimalTree(Board& _chessboard, color comp, int _targetDepth);
		struct Node {
			Node(float _boardValue);
			float boardValue, value_alphabeta;
		};
		color computerColor;
		int buildGameTreeMinimax(int depth, color side);
		int staticEvaluations;
		Board& chessBoard;
		unique_ptr<Node> Root;
		int targetDepth;
	};

	class fullTree // Tree containing NegaMax and Implementation
	{ 
	public:
		fullTree(Board& _chessboard, color comp, int _targetDepth);
		struct Node {
			Node();
			Move thisMove;
			int thisValue;
			vector<Move> moveList;
			vector<unique_ptr<Node>> nodeList;
		};
		color computerColor;
		int test_NegaMax(unique_ptr<Node>& node, int alpha, int beta, int depth, color side);
		int staticEvaluations, nalphaBeta, nHashLookups; // debug
		Board& chessBoard;
		unique_ptr<Node> Root;
		int targetDepth;
	};

	void testMinimalTree();
	void testFullTree();
	void testTreeStructure();
	void specialTest(); // unspecified custom test
	void testEvaluation();
	void testHashing();
	void testMagic();

private:
	void testPawnFill();
	void testCastling();
	void testEnpassent();
	void testProm();
};

#pragma optimize( "[optimization-list]", {on | off} )

class Benchmark
{
public:
	Benchmark();
	~Benchmark();
	void performAllbenchmarks();
	void summarize();

	void testPerft(int maxdepth);
	void perftTestSuite();
	void benchmarkMoveGeneration();
	void benchmarkMovemaking();
private:
	enum bType{MOVEGEN, MAKEMOVE};
	bool performingAll;
	struct result {
		bType type;
		string name;
		string msg;
		double value;
	};
	Board testBoard;
	vector<result> results;
	void perft(int maxDepth, const int targetDepth, color startColor);
	// perft statistics
	long perftNodeCount;
	long perftEPCount;
	long perftMoveCount;
	long perftCheckmateCount;

	long totalPerftMoveCount;
	long totalTotalPerftMoveCount;
};

class Timer
{
public:
	Timer();
	void start();
	void stop();
	double getTime(); // return microseconds
private:
	chrono::high_resolution_clock::time_point t1, t2;
};

#endif
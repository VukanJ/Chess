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
#include <boost/tokenizer.hpp>

#include "misc.h"
#include "Move.h"
#include "ZobristHash.h"
#include "Data.h"
#include "defines.h"
#include "AI.h"
#include "Board.h"

using namespace std;

typedef pair<string, string> stringpair;

class UnitTest
{
public:
	// Test game mechanics and calculations
	UnitTest();

	// Intrinsics testing
	void testDefines() const;
	void testIntrinsics() const;

	// Move generation testing
	void testGenerationAlgorithms();

	void specialTest(); // unspecified custom test
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
	void perft(int maxDepth, const int targetDepth, color startColor);
private:
	enum bType{MOVEGEN, MAKEMOVE};
	bool performingAll;
	struct result {
		bType type;
		string name;
		string msg;
		float value;
	};
	Board testBoard;
	vector<result> results;
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
	double getTime(); // returns microseconds
private:
	chrono::high_resolution_clock::time_point t1, t2;
};

class DataBaseTest
{
public:
	DataBaseTest();
	void start_Bratko_Kopec_Test();
private:
	int targetDepth;
	Move getBestMove(color forPlayer);
	Move distributeNegaMax(color forPlayer);
	int NegaMax(int alpha, int beta, int depth, color aiColor, color side);
	Board testBoard;
	ZobristHash transposition_hash;

	long evalcnt, negaMaxCnt, storedBoards, hashAccess, moveCnt;
};

// p n b r q 
constexpr int valueMapping[6] = {0, 3, 1, 2, 5, 4};
const vector<vector<int>> captureScore = {
	//       p    n    b    r    q
	/* p */ {104, 204, 304, 404, 504},
	/* n */ {103, 203, 303, 403, 503},
	/* b */ {102, 202, 302, 402, 502},
	/* r */ {101, 201, 301, 401, 501},
	/* q */ {100, 200, 300, 400, 500}
};

class SearchTest
{
public:
	SearchTest();
	void test(); // Custom function
	Move getBestMove(color forPlayer);
	int NegaMax(int alpha, int beta, int depth, color aiColor, color side);
	int QuiescenceSearch(int alpha, int beta, int depth, color aiColor, color side);
	void nextMove(MoveList&, const MoveList::iterator&, color);
	
	void extractPrincipalVariation(const U64& startKey, int maxPrintDepth, color aiColor);
	Move getMaxRootMove(color side);
	template<color side> bool isCheckmate();
private:
	int targetDepth;
	Board board;
	ZobristHash transpositionHash;
	long evalcnt, negaMaxCnt, storedBoards, hashAccess, moveCnt;
	double finished;
	double ordering;
};

#endif

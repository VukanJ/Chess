#ifndef TESTING_H
#define TESTING_H
#include <iostream>
#include <algorithm>
#include <memory>
#include <stack>
#include <climits>
#include <string>
#include <chrono>

#include "misc.h"
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
	class TestingTree {
	public:
		TestingTree(Board& _chessBoard, int _targetDepth);
		struct Node {
			Node(float _boardValue);
			float boardValue, value_alphabeta;
			vector<Move> moveList;
			vector<unique_ptr<Node>> nodeList;
		};
		void buildGameTree(unique_ptr<Node>& node, int depth, color side);
		void traceback();

		stack<Move> debugMoveStack;
		int staticEvaluations;
		Board& chessBoard;
		unique_ptr<Node> Root;
		int targetDepth;
	};

	class MinimalTree { // Low memory Tree, only stores current path
	public:
		MinimalTree(Board& _chessboard, color comp, int _targetDepth);
		struct Node {
			Node(float _boardValue);
			float boardValue, value_alphabeta;
		};
		color computerColor;
		float buildGameTreeMinimax(int depth, color side);
		int staticEvaluations;
		Board& chessBoard;
		unique_ptr<Node> Root;
		int targetDepth;
	};


	void testMinimalTree();
	void testTreeStructure();
	void specialTest(); // unspecified custom test
	void testEvaluation();

private:
	void testPawnFill();
	void testCastling();
	void testProm();
};

#pragma optimize( "[optimization-list]", {on | off} )

class Benchmark
{
public:
	Benchmark();
	void performAllbenchmarks();
	void summarize();

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
	vector<result> results;
};

#endif

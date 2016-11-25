#ifndef TESTING_H
#define TESTING_H
#include <iostream>
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
	void testDefines() const;
	void testIntrinsics() const;

	void testGenerationAlgorithms();
private:
	void testPawnFill();
	void testCastling();
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

#ifndef TESTING_H
#define TESTING_H
#include <iostream>
#include <climits>
#include <string>

#include "misc.h"
#include "Data.h"
#include "defines.h"

using namespace std;

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
};

#endif

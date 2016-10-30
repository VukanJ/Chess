#ifndef TESTING_H
#define TESTING_H
#include <iostream>
#include <climits>

#include "misc.h"
#include "Data.h"
#include "defines.h"

using namespace std;

class UnitTest
{
public:
	UnitTest();
	void testDefines() const;
	void testIntrinsics() const;
};


#endif

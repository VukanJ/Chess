#include "Testing.h"

UnitTest::UnitTest()
{

}

void UnitTest::testDefines()
{
	byte b = PIECE_PAIR(0xA,0x5);
	if (MOV_PIECE(b) != 0xA)    cerr << "TARGET_PIECE() Failed\n";
	if (TARGET_PIECE(b) != 0x5) cerr << "MOV_PIECE() Failed()\n";
}


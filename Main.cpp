#include <iostream>
#include <intrin.h>

#include "AI.h"
#include "gui.h"
#include "ZobristHash.h"
#include "Testing.h"

//const unsigned int WIDTH = 800, HEIGHT = 600; // GUI SIZE

int main()
{
	UnitTest test;
	test.testDefines();
	//AI computer("*");
	//AI computer("K788888R7R4k1R"); // checkmate check is not correct
	//AI computer("K1K2K1K/1p4p1/8/8/8/8/1P4P1/k1k2k1k");
	//AI computer("1K1BQ3/2P3R1/P2P4/P3Pq1R/2n1p3/1p1r1p2/8/1kr5");
	//AI computer("3KR38888882rk4"); //
	AI computer("RNBQ1RK1/PPPN1PPP/4P3/3Pp3/1B1p4/2nb1n2/ppp2ppp/r1bqk2r");
	computer.printDebug("prnbkqPRNBKQ");

	
	std::cin.ignore();
	return 0;
}
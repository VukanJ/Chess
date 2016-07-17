#include <iostream>
#include <intrin.h>

#include "AI.h"
#include "gui.h"
#include "ZobristHash.h"

const unsigned int WIDTH = 800, HEIGHT = 600;

int main()
{
	//AI computer("84Q");
	//AI computer("K788888R7R4k1R"); // checkmate check is not correct
	//AI computer("K1K2K1K/1p4p1/8/8/8/8/1P4P1/k1k2k1k");
	//AI computer("1K1BQ3/2P3R1/P2P4/P3Pq1R/2n1p3/1p1r1p2/8/1kr5");
	AI computer("R2K3R888888r2k3r"); // *
	//AI computer("8888888r2k4");
	computer.printDebug("RK");
	
	std::cin.ignore();
	return 0;
}
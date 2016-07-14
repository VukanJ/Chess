#include <iostream>
#include <intrin.h>

#include "AI.h"
#include "gui.h"
#include "ZobristHash.h"

const unsigned int WIDTH = 800, HEIGHT = 600;

int main()
{
	AI computer("K1PP4/P3PPBP/qPP3PN/8/5b2/pp2pQpp/k1p2pn1/2r2r2");
	//AI computer("K788888R7R4k1R"); // checkmate check is not correct
	//AI computer("4R33p4888888");
	//AI computer("*");
	computer.printDebug("pbnrkq");
	
	std::cin.ignore();
	return 0;
}
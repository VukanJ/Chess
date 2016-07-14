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
	//AI computer("4R33p4888888");
	AI computer("*");
	computer.printDebug("PBNRKQpbnrkq");
	
	std::cin.ignore();
	return 0;
}
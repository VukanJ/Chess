#include <iostream>
#include <intrin.h>

#include "AI.h"
#include "gui.h"

const unsigned int WIDTH = 800, HEIGHT = 600;

// __popcnt64
// _BitScanForward64(...,...)

int main()
{
	AI computer("r1b1k1nr/pp1pppbp/11n3p1/8/4PP2/2P5/PP1N2PP/R2Q1RK1");
	computer.printDebug("QrRKnN");
	std::cin.ignore();
	return 0;
}
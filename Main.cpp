#include <iostream>
#include <intrin.h>

#include "AI.h"
#include "gui.h"

const unsigned int WIDTH = 800, HEIGHT = 600;

// __popcnt64
// _BitScanForward64(...,...)

int main()
{
	AI computer;
	unsigned long index = -1;
	_BitScanReverse64(&index, (u64)1);
	cout << index << endl;
	std::cin.ignore();
	return 0;
}
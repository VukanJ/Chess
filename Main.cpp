#include <iostream>

#include "info.h"
#include "AI.h"
#include "UCI_client.h"
#include "ZobristHash.h"
#include "Testing.h"

using namespace std;

void testing();
void benchmark();

int main(int argc, char* argv[])
{
	sayHello();
	testing();

	AI computer("*", black);

	return 0;
}

void testing(){
	UCIclient uciClient;
	uciClient.UCI_IO_loop();
	//Benchmark benchmark;
	SearchTest searchTest;
	searchTest.test();
	//benchmark.benchmarkMovemaking();
	//benchmark.benchmarkMoveGeneration();
	//benchmark.testPerft(-1);
	//benchmark.perftTestSuite();

	//DataBaseTest dbt;
	//dbt.start_Bratko_Kopec_Test();

	cout << "EOP\n";
	cin.ignore();
	exit(0);
}

void benchmark()
{
	Benchmark bench;
	bench.performAllbenchmarks();
	bench.summarize();
}

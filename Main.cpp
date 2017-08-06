#include <iostream>

#include "info.h"
#include "AI.h"
#include "UCI_client.h"
#include "ZobristHash.h"
#include "consoleGame.h"
#include "Testing.h"

using namespace std;

void testing();

int main(int argc, char* argv[])
{
	sayHello();
	// Wait for first input to decide which mode should be started
	UCIclient uciClient;
	uciClient.UCI_IO_loop();
	while (true) {
		string input;
		getline(cin, input);
		boost::trim(input);
		if (input == "uci") {
			UCIclient uciClient;
			uciClient.printEngineID();
			uciClient.UCI_IO_loop();
		}
		else if (input == "xboard") { /* Not supported */ }
		else {
			// Interactive move
			ConsoleGame consoleGame;
			consoleGame.ioLoop(input);
		}
	}
	return 0;
}

void testing(){
	Benchmark benchmark;
	//SearchTest searchTest;
	//searchTest.test();
	//benchmark.benchmarkMovemaking();
	//benchmark.benchmarkMoveGeneration();
	//benchmark.testPerft(-1);
	benchmark.perftTestSuite();

	//DataBaseTest dbt;
	//dbt.start_Bratko_Kopec_Test();

	cout << "EOP\n";
	cin.ignore();
	exit(0);
}

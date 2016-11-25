#include <iostream>

#include "AI.h"
//#include "gui.h"
#include "ZobristHash.h"
#include "Testing.h"

//#define GUI_MODE

// TODO: Write test functions for all move generating functions

const unsigned int WIDTH = 800, HEIGHT = 600; // GUI SIZE

void testing();
void benchmark();

int main()
{
	testing();

	//AI computer("*");
	//AI computer("K788888R7R4k1R"); // checkmate check is not correct
	AI computer("* b - 1 0", white);
	//AI computer("  1K1BQ3/2P3R1/P2P4/P3Pq1R/2n1p3/1p1r1p2/8/1kr5 w KkQq 1 0", black);
	//AI computer("r3k2rpppppppp8888PPPPPPPPR3K2R w KkQq 1 0", black); // Test Castling
	//AI computer("RNBQ1RK1/PPPN1PPP/4P3/3Pp3/1B1p4/2nb1n2/ppp2ppp/r1bqk2r");
	//AI computer("84P3888888");
	//AI computer("R1BKQBNR/P1PPPPPP/1PP5/84q3/6p1/ppppppbp/rnbkq1nr");
	computer.printDebug();


#ifdef GUI_MODE
	sf::RenderWindow window(sf::VideoMode(800, 600), "Chess GUI", sf::Style::Close);
	window.setFramerateLimit(60);
	Gui gui;

	while (window.isOpen()){
		sf::Event ev;
		while (window.pollEvent(ev)){
			switch (ev.type){
				case sf::Event::Closed: window.close(); break;
				case sf::Event::KeyPressed:
					switch (ev.key.code){
						case sf::Keyboard::Escape:
							window.close();break;
					}
					break;
			}
		}
		window.clear(sf::Color::Black);
		gui.render(window,computer.getBoardRef());
		window.display();
	}
#endif

	std::cin.ignore();
	return 0;
}

void testing(){
	UnitTest test;
	test.testDefines();
	test.testIntrinsics();

	test.testGenerationAlgorithms();
		
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

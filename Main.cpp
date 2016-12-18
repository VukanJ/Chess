#include <iostream>
#include <SFML/Graphics.hpp>

#include "AI.h"
#include "gui.h"
#include "ZobristHash.h"
#include "Testing.h"

using namespace std;

#define GUI_MODE

const unsigned int WIDTH = 800, HEIGHT = 600; // GUI SIZE

//void testing();
//void benchmark();


int main()
{
	//testing();
	//AI computer("*");
	//AI computer("K788888R7R4k1R"); // checkmate check is not correct
	//AI computer("* b - 1 0", white);
	//AI computer("2rq1rk1/pb1n1ppN/4p3/1pb5/3P1Pn1/P1N5/1PQ1B1PP/R1B2RK1 w KkQq 1 0", black);
	AI computer("2Q5/p3b1k1/2p2rPp/2q5/4B3/3P4/P6P/6RK w KkQq 1 0", black);
	//AI computer("r3k2rpppppppp8888PPPPPPPPR3K2R w KkQq 1 0", black); // Test Castling
	//AI computer("RNBQ1RK1/PPPN1PPP/4P3/3Pp3/1B1p4/2nb1n2/ppp2ppp/r1bqk2r");
	//AI computer("84P3888888");
	//AI computer("R1BKQBNR/P1PPPPPP/1PP5/84q3/6p1/ppppppbp/rnbkq1nr");
#ifdef GUI_MODE
	sf::RenderWindow window(sf::VideoMode(800, 600), "Chess GUI", sf::Style::Close);
	window.setFramerateLimit(60);
	Gui gui(computer.getBoardPtr());

	while (window.isOpen()){
		sf::Event ev;
		while (window.pollEvent(ev)){
			switch (ev.type){
				case sf::Event::Closed: window.close(); break;
				case sf::Event::KeyPressed:
					switch (ev.key.code){
						case sf::Keyboard::Escape:
							window.close();break;
						default:
							gui.handleEvent(ev, window);
							break;
					}
					break;
				default:
					gui.handleEvent(ev, window);
					break;
			}
		}
		window.clear(sf::Color::Black);
		gui.render(window);
		window.display();
	}
#endif

	std::cin.ignore();
	return 0;
}

void testing(){
	UnitTest test;
	test.specialTest();
	test.testDefines();
	test.testIntrinsics();
	test.testGenerationAlgorithms();
	//test.testTreeStructure();
	test.testEvaluation();
	test.testFullTree();
	test.testHashing();
	//test.testMinimalTree();

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


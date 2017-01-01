#include <iostream>
#include <SFML/Graphics.hpp>

#include "AI.h"
#include "gui.h"
#include "ZobristHash.h"
#include "Testing.h"

// TODO: Write Testing function for Cprom

using namespace std;

#define GUI_MODE

const unsigned int WIDTH = 800, HEIGHT = 600; // GUI SIZE

void testing();
void benchmark();

int main()
{
	//testing();
	//AI computer("4k3pppppppp8888PPPPPPPP4K4 w - 1 0", black);
	//AI computer("r1b1r1k1/1p1n1p1p/2ppnqp1/4b3/p1P1P3/5P2/PPNQNBPP/1R2RBK1 w - 1 0",black);
	//AI computer("81p1p1p1p8PPPNNN7pPp53p1P8 w - 1 0", black);
	//AI computer("1rr4r/2r2rrr/r1r2rr1/8/rR3R2/6R1/RRR2RRR/1R1RR111 w - 1 0", black);
	//AI computer("2rq1rk1/pb1n1ppN/4p3/1pb5/3P1Pn1/P1N5/1PQ1B1PP/R1B2RK1 w - 1 0", black);
	AI computer("4r3/7q/nb2prRp/pk1p3P/3P4/P7/1P2N1P1/1K1B1N2 w - 1 0", black); // Mate in 5

	//AI computer("8/b1b5/1P2n1b1/1P3P11/8887R w - 1 0", black);
	
	//AI computer("2Q5/4b1k1/1Pp2rPp/2q5/4Bn2/pppp4/P6P/6RK w - 1 0", black); 
	//AI computer("rk5r/pp1Q1p1p/1q1p1N2/88/6P1/PP3PBP/2R3K1 w - 1 0", white); // Mate in 2 puzzle
	
	//AI computer("* w kKqQ 1 0", black);
	
	//AI computer("RNBQ1RK1/PPPN1PPP/4P3/3Pp3/1B1p4/2nb1n2/ppp2ppp/r1bqk2r");
	//AI computer("R1BKQBNR/P1PPPPPP/1PP5/84q3/6p1/ppppppbp/rnbkq1nr");

#ifdef GUI_MODE
	sf::RenderWindow window(sf::VideoMode(800, 600), "Chess GUI", sf::Style::Close);
	window.setFramerateLimit(60);

	Gui gui(computer, computer.aiColor);
	computer.printBoard();
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
					if (gui.handleEvent(ev, window)) {
						// Human player played a valid move
						gui.render(window);
						window.display();
						computer.Play();
					}
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
	test.testTreeStructure();
	test.testEvaluation();
	//test.testFullTree();
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


#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <streambuf>
#include <SFML/Graphics.hpp>

#include "AI.h"
#include "gui.h"
#include "ZobristHash.h"
#include "Testing.h"

const string VERSION = "1.0";

// TODO: Write Testing function for Cprom

using namespace std;

#define GUI_MODE

const unsigned int WIDTH = 800, HEIGHT = 600; // GUI SIZE

void sayHello()
{
		cout << string(53, '~') << endl;
		cout << left;
		cout << setw(52) << string("| \"VChess\" Version " + VERSION) << "|\n";
		cout << setw(52) << "| Author: VukanJ -- https://github.com/VukanJ" << "|\n";
		cout << setw(52) << "| Press <C> for credits" << "|\n";
		cout << setw(52) << "| Press <L> for license" << "|\n";
		cout << string(53, '~') << endl;
}

void showLicense()
{
	ifstream licenseFile("License.txt", ios::in);
	if(!licenseFile.is_open()){
		cout << "License file not found.\n";
		cout << "Visit https://github.com/VukanJ/Chess for the full License\n";
		return;
	}
	string licenseText((istreambuf_iterator<char>(licenseFile)),
										 istreambuf_iterator<char>());
	licenseFile.close();
	cout << licenseText << endl;
}

void showCredits()
{
	//cout << string(10,'~') << "CREDITS" << string(10,'~');
	cout << setfill('-') << left;
	cout << setw(30) << "Author"  << "VukanJ (github.com/VukanJ)\n";
	cout << setw(30) << "Fonts"   << "FreeFonts.org\n";
	cout << setw(30) << "Sprites" << "jurgenwesterhof\n";
	cout << setw(30) << "Sprite Source" << "wikimedia.org/wiki/Template:SVG_chess_pieces\n";
	cout << setw(30) << "Language" << "C++" << endl;
//	printf(string(10,'~') + "EXTERNAL SOFTWARE" + string(10,'~'));
	cout << setw(30) << "SFML" << "Simple and fast multimedia library\n";
	cout << setw(30) << "Boost" << "C++ Boost\n";
}

void testing();
void benchmark();

int main()
{
	sayHello();
	//testing();
	//AI computer("4k3pppppppp8888PPPPPPPP4K4 w - 1 0", black);
	//AI computer("r1b1r1k1/1p1n1p1p/2ppnqp1/4b3/p1P1P3/5P2/PPNQNBPP/1R2RBK1 w - 1 0",black);
	//AI computer("81p1p1p1p8PPPNNN7pPp53p1P8 w - 1 0", black);
	//AI computer("1rr4r/2r2rrr/r1r2rr1/8/rR3R2/6R1/RRR2RRR/1R1RR111 w - 1 0", black);

	AI computer("rnbqkbnr/pppppppp/8888/PPPPPPPP/RNBQKBNR w - 1 0", black);

	//AI computer("4r3/7q/nb2prRp/pk1p3P/3P4/P7/1P2N1P1/1K1B1N2 w - 1 0", black); // Mate in 5

	//AI computer("8/b1b5/1P2n1b1/1P3P11/8887R w - 1 0", black);

	//AI computer("2Q5/4b1k1/1Pp2rPp/2q5/4Bn2/pppp4/P6P/6RK w - 1 0", black);
	//AI computer("rk5r/pp1Q1p1p/1q1p1N2/88/6P1/PP3PBP/2R3K1 w - 1 0", white); // Mate in 2 puzzle

	//AI computer("* w kKqQ 1 0", black);

	//AI computer("RNBQ1RK1/PPPN1PPP/4P3/3Pp3/1B1p4/2nb1n2/ppp2ppp/r1bqk2r");
	//AI computer("R1BKQBNR/P1PPPPPP/1PP5/84q3/6p1/ppppppbp/rnbkq1nr");

	//computer.Play();

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
						case sf::Keyboard::L:
							showLicense();
							break;
						case sf::Keyboard::C:
							showCredits();
							break;
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
	//test.testTreeStructure();
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

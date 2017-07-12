#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Window/ContextSettings.hpp>

#include "info.h"
#include "AI.h"
//#include "gui.h"
#include "ZobristHash.h"
#include "Testing.h"

using namespace std;

//#define GUI_MODE

const uint WIDTH = 800, HEIGHT = 600; // GUI SIZE

void testing();
void benchmark();

int main(int argc, char* argv[])
{
	sayHello();
	testing();

	AI computer("*", black);
#ifdef GUI_MODE
	sf::RenderWindow window(sf::VideoMode(800, 600), "Chess", sf::Style::Close, sf::ContextSettings(0, 0, 3));
	window.setFramerateLimit(60);

	Gui gui(computer, computer.aiColor);

	computer.bindGui(&gui);
	//computer.Play(window);

	sf::RectangleShape testoverlay;
	testoverlay.setSize(sf::Vector2f(WIDTH, HEIGHT));
	testoverlay.setPosition(sf::Vector2f(0, 0));
	testoverlay.setFillColor(sf::Color(30, 30, 30, 130));

	sf::Clock clock;
	while (window.isOpen()){
		float frameTime = clock.restart().asSeconds();
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
						computer.writeToHistory(gui.lastMove);
						gui.render(window);
						window.display();
						computer.Play(window);
						frameTime = 0;
					}
					break;
			}
		}
		window.clear(sf::Color::Black);
		gui.update(frameTime);
		gui.render(window);
		//window.draw(testoverlay);
		window.display();
	}
#endif
	cin.ignore();
	return 0;
}

void testing(){

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

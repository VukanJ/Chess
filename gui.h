#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "defines.h"
#include "Board.h"

using namespace std;

extern const unsigned int WIDTH, HEIGHT;

class Gui
{
public:
	Gui(const Board* _board);
	void render(sf::RenderWindow& window);
	void handleEvent(sf::Event& ev, sf::RenderWindow& window);
private:
	struct UserInput {
		int from, to;
		bool pieceSelected;
		piece movePiece, targetPiece;
	};
	UserInput userInput;
	bool isInputMoveValid();
	enum {
		K=5, Q=4, B=1, N=2, R=3, P=0,
		k=11, q=10, b=7, n=8, r=9, p=6
	};
	const Board* chessBoard;

	sf::Image boardImage;
	sf::Texture boardTex;
	sf::Sprite boardspr;

	sf::Texture pieceTex;
	vector<sf::Sprite> pieces;

	float size;             // apparent size of pieces
	sf::Vector2f orig_size; // original size of pieces
	sf::Vector2f scale;     // scaling factor

	enum{debugText, positionText, clickText};
	sf::Font textFont;
	vector<sf::Text> textDisplays;
};

#endif

#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "defines.h"
#include "AI.h"
#include "Board.h"

using namespace std;

extern const unsigned int WIDTH, HEIGHT;

class Gui
{
public:
	Gui(AI& _ai, color aiColor);

	void render(sf::RenderWindow& window);
	bool handleEvent(sf::Event& ev, sf::RenderWindow& window);
private:
	struct UserInput {
		int from;
		bool pieceSelected;
		piece movePiece;
		void reset();
	};
	UserInput userInput;
	bool isUserMoveValid_completeMoveInfo(Move& userMove);
	void colorSquares(u64 pattern, sf::Color color, sf::RenderWindow& window);

	enum options {drawAttB = 0x1, drawAttW = 0x2};
	u32 drawOptions;

	Board& chessBoard;

	sf::Image boardImage;
	sf::Texture boardTex;
	sf::Sprite boardspr;

	sf::Texture pieceTex;
	vector<sf::Sprite> pieces;

	float size;             // apparent size of pieces
	sf::Vector2f orig_size; // original size of pieces
	sf::Vector2f scale;     // scaling factor

	enum{debugText, positionText, clickText, moveListText};
	void debugDrawSquareNumering(sf::RenderWindow& window);
	sf::Font textFont;
	vector<sf::Text> textDisplays;

	color humanColor;
};

#endif

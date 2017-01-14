#define _USE_MATH_DEFINES
#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <vector>
#include <string>

#include "defines.h"
#include "AI.h"
#include "Board.h"

using namespace std;

extern const unsigned int WIDTH, HEIGHT;

class AI;

class Gui
{
public:
	Gui(AI& _ai, color aiColor);

	void update(float frameTime);
	void render(sf::RenderWindow& window);
	bool handleEvent(sf::Event& ev, sf::RenderWindow& window);
	void visualizeScore(int ChessScore);
	void visualizeLastMove(const Move& move);
	void showMessage(const string&, sf::Color); // Show game info (invalid move, check etc. )

	Move lastMove; // Updated after each move from human player
private:
	class ScoreGauge
	{
	public:
		// Visual representation of the score
		ScoreGauge();
		void update(int ChessScore, color side);
		void render(sf::RenderWindow& window);
	private:
		sf::Font font;
		sf::Text scoreText;
		sf::ConvexShape blackSide;
		sf::ConvexShape whiteSide;
		sf::CircleShape midCircle;
		//sf::RectangleShape hide;
		sf::Vector2f origin;
	} scoreGauge;
	
	class Arrow {
		// Arrow shows last move of computer
	public:
		Arrow();
		void setMove(const Move& move);
		void update(float frameTime);
		void render(sf::RenderWindow& window);
		float fadelevel;
	private:
		float elapsedTime;
		sf::RectangleShape arrowBody;
		sf::ConvexShape    arrowHead;
	} guiArrow;

	struct UserInput {
		int from;
		bool pieceSelected;
		piece movePiece;
		void reset();
	} userInput;

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

	// Display texts
	enum{history_text, mate_in_text};
	void debugDrawSquareNumering(sf::RenderWindow& window);
	sf::Font textFont;
	vector<sf::Text> textDisplays;
	vector<pair<sf::Text, float>> messages; // Text and fadeout-timer

	color humanColor;
};

#endif

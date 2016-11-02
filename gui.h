#ifndef GUI_H
#define GUI_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "Board.h"

using namespace std;

extern const unsigned int WIDTH, HEIGHT;

// class "owned" by class AI
/*
class Gui
{
public:
	Gui();
	void render(sf::RenderWindow& window, const Board&);
private:
	enum {
		K, Q, B, N, R, P,
		k, q, b, n, r, p
	};
	sf::Image boardImage;
	sf::Texture boardTex;
	sf::Sprite boardspr;

	sf::Texture pieceTex;
	vector<sf::Sprite> pieces;

	float size;             // apparent size of pieces
	sf::Vector2f orig_size; // original size of pieces
	sf::Vector2f scale;     // scaling factor
};
*/
#endif

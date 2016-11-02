#include "gui.h"
/*
Gui::Gui()
{
	// Draw board image (8 x 8) pixel
	srand(time(0));
	boardImage.create(8, 8, sf::Color::Black);
	auto c = 1;
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			boardImage.setPixel(j, i, c++ % 2 == 0 ? sf::Color(70, 60, 60) : sf::Color(230, 230, 230));
		}
		c++;
	}
	boardTex.loadFromImage(boardImage);
	boardspr.setTexture(boardTex);
	boardspr.setScale(sf::Vector2f(HEIGHT / 8, HEIGHT / 8));

	// Load Textures, crop sprites
	pieceTex.loadFromFile("Chess_Pieces_Sprite.png");
	pieces = vector<sf::Sprite>(12, sf::Sprite(pieceTex));
	size = HEIGHT / 8;
	orig_size = sf::Vector2f((float)pieceTex.getSize().x / 6.0f, (float)pieceTex.getSize().y / 2.0f);
	scale = sf::Vector2f(size / orig_size.x, size / orig_size.y);

	for (int i = 6; i < 12; i++)
		pieces[i].setTextureRect(sf::IntRect(orig_size.x*(i - 6), orig_size.y, orig_size.x, orig_size.y));
	for (int i = 0; i < 6; i++)
		pieces[i].setTextureRect(sf::IntRect(orig_size.x*i, 0, orig_size.x, orig_size.y));

	for (auto& p : pieces)
		p.setScale(scale.x, scale.y);

	// Sprite correction

}

void Gui::render(sf::RenderWindow& window, const Board& board)
{
	window.draw(boardspr);
	int pieceIndex = 0;
	unsigned long pos = 0;
	for (const auto& type : board.pieces){
		auto mask = type;
		do{
			BITSCANR64(pos, mask);
			mask ^= 1ull << pos;
			pieces[pieceIndex].setPosition(((63-pos) % 8)*orig_size.y*scale.y, ((63-pos) / 8)*orig_size.x*scale.x);
			window.draw(pieces[pieceIndex]);
		}while (pos);
		pieceIndex++;
	}
}
*/
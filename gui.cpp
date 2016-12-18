#include "gui.h"

Gui::Gui(const Board* _board) : chessBoard(_board)
{
	// Draw board image (8 x 8) pixel
	srand(time(0));
	boardImage.create(8, 8, sf::Color::Black);
	auto c = 1;
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			boardImage.setPixel(j, i, c++ % 2 == 0 ? sf::Color(80, 70, 70) : sf::Color(230, 230, 230));
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

	string pngOrdering = "PBNRQKpbnrqk";

	for (int i = 0; i < 6; i++)
		pieces[getPieceIndex(pngOrdering[i])].setTextureRect(sf::IntRect(orig_size.x*i, 0, orig_size.x, orig_size.y));
	for (int i = 6; i < 12; i++)
		pieces[getPieceIndex(pngOrdering[i])].setTextureRect(sf::IntRect(orig_size.x*(i - 6), orig_size.y, orig_size.x, orig_size.y));

	for (auto& p : pieces)
		p.setScale(scale.x, scale.y);
	
	userInput.pieceSelected = false;
	// sf::Text

	if (!textFont.loadFromFile("lucon.ttf")) {
		cout << "Font not found!\n";
		exit(1);
	}
	textDisplays.push_back(sf::Text("Debug", textFont));
	textDisplays[debugText].setPosition(600, 0);
	textDisplays[debugText].setCharacterSize(12);
	textDisplays.push_back(sf::Text("Position", textFont));
	textDisplays[positionText].setPosition(600, 20);
	textDisplays[positionText].setCharacterSize(12);
	textDisplays.push_back(sf::Text("Klick", textFont));
	textDisplays[clickText].setPosition(600, 40);
	textDisplays[clickText].setCharacterSize(12);
}

void Gui::render(sf::RenderWindow& window)
{
	window.draw(boardspr);
	int pieceIndex = 0;
	unsigned long pos = 0;
	for (const auto& type : chessBoard->pieces){
		auto mask = type;
		BITLOOP(pos, mask) {
			pieces[pieceIndex].setPosition(((63 - pos) % 8)*orig_size.y*scale.y, ((63 - pos) / 8)*orig_size.x*scale.x);
			window.draw(pieces[pieceIndex]);
		}
		pieceIndex++;
	}
	for (auto& text : textDisplays) {
		window.draw(text);
	}
}

void Gui::handleEvent(sf::Event& ev, sf::RenderWindow& window)
{
	sf::Vector2i mouse;
	string assemble;
	switch (ev.type) {
	case sf::Event::MouseMoved:
		mouse = sf::Mouse::getPosition(window);
		assemble = "Mouse at (" + to_string(mouse.x) + ',' + to_string(mouse.y) + ')';
		textDisplays[positionText].setString(assemble);
		break;
	case sf::Event::MouseButtonPressed:
		mouse = sf::Mouse::getPosition(window);
		if (mouse.x > HEIGHT) break;
		assemble = "Klick at (" + to_string(mouse.x) + ',' + to_string(mouse.y) + ") = ";
		assemble += 'a' + (mouse.x / (HEIGHT / 8));
		assemble += '8' - (mouse.y / (HEIGHT / 8));

		auto selectedSquare = -1;
		selectedSquare = (mouse.x / (HEIGHT / 8));
		selectedSquare += 8 * (mouse.y / (HEIGHT / 8));
		selectedSquare = 63 - selectedSquare;

		if (userInput.pieceSelected) userInput.to = selectedSquare;
		else userInput.from = selectedSquare;

		if (chessBoard->allPos & BIT_AT(selectedSquare)) {
			for (int p = 0; p < 12; p++) {
				if (chessBoard->pieces[p] & BIT_AT(selectedSquare)) {
					if (userInput.pieceSelected) {
						userInput.targetPiece = (piece)p;
					}
					else {
						userInput.movePiece = (piece)p;
					}
					break;
				}
			}
		}
	
		textDisplays[clickText].setString(assemble);
		break;
	}
}
#include "gui.h"

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
	board.setTexture(boardTex);
	board.setScale(sf::Vector2f(HEIGHT / 8, HEIGHT / 8));

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
}

void Gui::render(sf::RenderWindow& window, string FEN)
{
	// Render only needs to be called, when changes in position occur
	window.draw(board);
	auto pos = -1;
	for (auto& w : FEN){
		auto token = -1;
		if (w > 47 && w < 58)
			pos += w - 48;
		else switch (w){
			case 'r': token = r; break;
			case 'n': token = n; break;
			case 'b': token = b; break;
			case 'k': token = k; break;
			case 'q': token = q; break;
			case 'p': token = p; break;

			case 'R': token = R; break;
			case 'N': token = N; break;
			case 'B': token = B; break;
			case 'K': token = K; break;
			case 'Q': token = Q; break;
			case 'P': token = P; break;
			case '/':break;
			default:
				cerr << "BAD FEN (Board GUI)!";
		}
		if (token > -1){
			pos++;
			pieces[token].setPosition(size*(pos % 8), size*(pos / 8));
			window.draw(pieces[token]);
		}
	}
}

#include "gui.h"

Gui::Gui(const Board* _board, color aiColor) : chessBoard(_board)
{
	humanColor = aiColor == black ? white : black;

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

		if (mouse.x > HEIGHT || mouse.y > HEIGHT) break;

		auto selectedSquare = -1;
		selectedSquare = 63 - ((mouse.x / (HEIGHT / 8)) + 8 * (mouse.y / (HEIGHT / 8)));

		assemble = "Klick at (" + to_string(mouse.x) + ',' + to_string(mouse.y) + ") = ";
		assemble += 'h' - selectedSquare % 8;
		assemble += '1' + selectedSquare / 8;

		piece pieceClicked = nullPiece;

		if (chessBoard->allPos & BIT_AT(selectedSquare)) { // User clicked on piece ? 
			for (int p = 0; p < 12; p++) {
				if (chessBoard->pieces[p] & BIT_AT(selectedSquare)) {
					pieceClicked = (piece)p;
					break;
				}
			}
		}

		// 
		Move user_GUI_Move; // Probably incorrect, since user makes mistakes
		// Only contains raw data, without castling rights or specific attack types

		if (pieceClicked == nullPiece) { // No piece selected
			if (userInput.pieceSelected) { // May be a quiet move
				user_GUI_Move.from   = userInput.from;
				user_GUI_Move.to     = selectedSquare;
				user_GUI_Move.flags  = MOVE;
				user_GUI_Move.Pieces = userInput.movePiece;
				if (!isUserMoveValid_completeMoveInfo(user_GUI_Move)) {
					cerr << "Invalid move!\n";
				}
				else {
					cerr << "OK!\n";
				}
				userInput.reset();
			}
		}
		else { // Clicked on piece
			if (userInput.pieceSelected) { // May be a capture

				userInput.reset();
			}
			else { // Nothing, but new piece was selected
				userInput.pieceSelected = true;
				userInput.movePiece = pieceClicked;
				userInput.from = selectedSquare;
			}
		}
		break;
	}
	textDisplays[clickText].setString(assemble);
}

bool Gui::isUserMoveValid_completeMoveInfo(Move& inputMove)
{
	// Checks if selected move can be played. If it can, 
	// the needed move metadata is filled in correctly.
	// -> Generates all possible moves and tries to find a match.
	
	vector<Move> possibleMoves;
	chessBoard->generateMoveList(possibleMoves, humanColor);

	vector<Move>::iterator matchingMove = find_if(possibleMoves.begin(), possibleMoves.end(), [&](Move& pmove) {
		if (inputMove.flags == WCASTLE   || 
			inputMove.flags == WCASTLE_2 || 
			inputMove.flags == BCASTLE   || 
			inputMove.flags == BCASTLE_2) {
			// Some type of castling
			if (pmove.flags == inputMove.flags) { // Is castling side (and color) matching ?
				// Player can castle
				return true;
			}
		}
		else if (pmove.from == inputMove.from && pmove.to == inputMove.to) {
			// Strong indicator for correct move
			if (pmove.flags == MOVE && inputMove.flags == MOVE) {
				// quiet move is allowed
				return true;
			}
			else {
				if (inputMove.Pieces == pmove.Pieces) {
					// Move allowed. WIP for Enpassent
					return true;
				}
			}
		}
		return false; // Illegal move detected
	});
	// Copy move metadata if match was found
	if (matchingMove == possibleMoves.end()) {
		return false;
	}
	else {
		inputMove = *matchingMove;
		return true;
	}
}

void Gui::UserInput::reset()
{
	from = nullSquare;
	pieceSelected = false;
	movePiece = nullPiece;
}
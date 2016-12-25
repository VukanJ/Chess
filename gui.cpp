#include "gui.h"

Gui::Gui(AI& _ai, color aiColor) : chessBoard(_ai.chessBoard)
{
	humanColor = aiColor == black ? white : black;
	drawOptions = 0x0;

	// Draw board image (8 x 8) pixel
	srand((unsigned int)time((time_t)(0)));
	boardImage.create(8, 8, sf::Color::Black);
	auto c = 1;
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			boardImage.setPixel(j, i, c++ % 2 == 0 ? sf::Color(120, 120, 120) : sf::Color(23, 230, 230));
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
	textDisplays.push_back(sf::Text("MOVE", textFont));
	textDisplays[moveListText].setPosition(600, 60);
	textDisplays[moveListText].setCharacterSize(10);
}

void Gui::render(sf::RenderWindow& window)
{
	window.draw(boardspr);
	debugDrawSquareNumering(window);
	if (drawOptions & drawAttB) {
		colorSquares(chessBoard.blackAtt, sf::Color(0,0,255,100), window);
	}
	if (drawOptions & drawAttW) {
		colorSquares(chessBoard.whiteAtt, sf::Color(255, 0, 0, 200), window);
	}
	int pieceIndex = 0;
	unsigned long pos = 0;
	for (const auto& type : chessBoard.pieces){
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
	Move user_GUI_Move; // Probably incorrect, since user makes mistakes
						// Only contains raw data, without castling rights or specific attack types
	piece pieceClicked = nullPiece;
	auto selectedSquare = -1;
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

		selectedSquare = 63 - ((mouse.x / (HEIGHT / 8)) + 8 * (mouse.y / (HEIGHT / 8)));

		assemble = "Klick at (" + to_string(mouse.x) + ',' + to_string(mouse.y) + ") = ";
		assemble += 'h' - selectedSquare % 8;
		assemble += '1' + selectedSquare / 8;


		if (chessBoard.allPos & bit_at(selectedSquare)) { // User clicked on piece ? 
			for (int p = 0; p < 12; p++) {
				if (chessBoard.pieces[p] & bit_at(selectedSquare)) {
					pieceClicked = (piece)p;
					break;
				}
			}
		}

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
					chessBoard.makeMove(user_GUI_Move, humanColor);
					chessBoard.updateAllAttacks();
					chessBoard.print();
					printBitboard(chessBoard.blackAtt);
				}
				userInput.reset();
			}
		}
		else { // Clicked on piece
			if (userInput.pieceSelected) { // May be a capture
				user_GUI_Move.from   = userInput.from;
				user_GUI_Move.to     = selectedSquare;
				user_GUI_Move.flags  = CAPTURE; // some kind of capture
				user_GUI_Move.Pieces = piece_pair(userInput.movePiece, pieceClicked);
				if (!isUserMoveValid_completeMoveInfo(user_GUI_Move)) {
					cerr << "Invalid move!\n";
				}
				else {
					cerr << "OK!\n";
					chessBoard.makeMove(user_GUI_Move, humanColor);
					chessBoard.updateAllAttacks();
					chessBoard.print();
					printBitboard(chessBoard.blackAtt);
				}
				userInput.reset();
			}
			else { // Nothing, but new piece was selected
				userInput.pieceSelected = true;
				userInput.movePiece = pieceClicked;
				userInput.from = selectedSquare;
			}
		}
		textDisplays[clickText].setString(assemble);
		break;
	case sf::Event::KeyPressed:
		switch (ev.key.code) {
		case sf::Keyboard::F1:
			drawOptions = drawOptions & drawAttB ? drawOptions & ~drawAttB : drawOptions | drawAttB;
			break;
		case sf::Keyboard::F2:
			drawOptions = drawOptions & drawAttW ? drawOptions & ~drawAttW : drawOptions | drawAttW;
			break;
		}
		break;
	}
}

bool Gui::isUserMoveValid_completeMoveInfo(Move& inputMove)
{
	// Checks if selected move can be played. If it can, 
	// the needed move metadata is filled in correctly.
	// -> Generates all possible moves and tries to find a match.
	
	vector<Move> possibleMoves, opponentPossibleMoves;
	chessBoard.generateMoveList(possibleMoves, humanColor);
	chessBoard.generateMoveList(opponentPossibleMoves, humanColor == black ? white : black); // For Debugging

	string moveStringList;
	for (int i = 0; i < max(opponentPossibleMoves.size(), possibleMoves.size()); i++) {
		if (i >= possibleMoves.size()) moveStringList += "   \t";
		else moveStringList += moveString(possibleMoves[i]) + '\t';
		if (i >= opponentPossibleMoves.size()) moveStringList += "   \n";
		else moveStringList += moveString(opponentPossibleMoves[i]) + '\n';
	}
	textDisplays[moveListText].setString(moveStringList);

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
			else if (pmove.flags == CAPTURE && inputMove.flags == CAPTURE) {
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

void Gui::colorSquares(u64 pattern, sf::Color color, sf::RenderWindow& window)
{
	sf::RectangleShape colorRect(sf::Vector2f(HEIGHT / 8, HEIGHT / 8));
	colorRect.setFillColor(color);
	BITLOOP(pos, pattern) {
		colorRect.setPosition(((-(int)pos + 63) % 8)*HEIGHT / 8, ((-(int)pos + 63) / 8)*HEIGHT / 8);
		//colorRect.setPosition(((-1+63) % 8)*HEIGHT / 8, ((-1+63) / 8)*HEIGHT / 8);
		window.draw(colorRect);
	}
}

void Gui::debugDrawSquareNumering(sf::RenderWindow& window)
{
	sf::Text Sq_Number("?", textFont);
	Sq_Number.setFillColor(sf::Color(0, 0, 0, 20));
	Sq_Number.setCharacterSize(60);
	for (int i = 0; i < 64; i++) {
		Sq_Number.setString(to_string(i));
		Sq_Number.setPosition(sf::Vector2f(((63 - i) % 8)*HEIGHT / 8, ((63 - i) / 8)*HEIGHT / 8));
		window.draw(Sq_Number);
	}
}
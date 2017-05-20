#include "gui.h"

Gui::Gui(AI& _ai, color aiColor) : chessBoard(_ai.chessBoard)
{
	humanColor = aiColor == black ? white : black;
	drawOptions = 0x0;

	// Draw board image (8 x 8) pixel
	boardImage.create(8, 8, sf::Color::Black);
	auto c = 1;
	for (int i = 0; i < 8; i++){
		for (int j = 0; j < 8; j++){
			boardImage.setPixel(j, i, c++ % 2 == 0 ? sf::Color(173, 135, 94) : sf::Color(234, 215, 179));
		}
		c++;
	}
	boardTex.loadFromImage(boardImage);
	boardspr.setTexture(boardTex);
	boardspr.setScale(sf::Vector2f(HEIGHT / 8.0f, HEIGHT / 8.0f));

	// Load Textures, crop sprites
	pieceTex.loadFromFile("Chess_Pieces_Sprite.png");
	pieces = vector<sf::Sprite>(12, sf::Sprite(pieceTex));
	size = HEIGHT / 8;
	orig_size = sf::Vector2f((float)pieceTex.getSize().x / 6.0f, (float)pieceTex.getSize().y / 2.0f);
	scale = sf::Vector2f(size / orig_size.x, size / orig_size.y);

	string pngOrdering = "KQBNRPkqbnrp";

	for (int i = 0; i < 6; i++)
		pieces[getPieceIndex(pngOrdering[i])].setTextureRect(sf::IntRect(orig_size.x*i, 0, orig_size.x, orig_size.y));
	for (int i = 6; i < 12; i++)
		pieces[getPieceIndex(pngOrdering[i])].setTextureRect(sf::IntRect(orig_size.x*(i - 6), orig_size.y, orig_size.x, orig_size.y));

	for (auto& p : pieces)
		p.setScale(scale.x, scale.y);

	userInput.pieceSelected = false;
	// sf::Text

	if (!textFont.loadFromFile("FiraMono-Regular.otf")) {
		cout << "Font not found!\n";
		exit(1);
	}
}

void Gui::update(float frametime)
{
	static float elapsedTime = 0;
	elapsedTime += frametime;
	sf::Color tempCol;
	// Text fades out and is then removed
	for (auto msg = messages.begin(); msg != messages.end();) {
		msg->second += frametime;
		tempCol = msg->first.getFillColor();
		tempCol.a = -10*msg->second + 255.0f;
		msg->first.setFillColor(tempCol);
		if (tempCol.a <= 10) msg = messages.erase(messages.begin());
		else msg++;
	}
	guiArrow.update(frametime);
}

void Gui::render(sf::RenderWindow& window)
{
	window.draw(boardspr);
	if (drawOptions & drawAttB) {
		colorSquares(chessBoard.attacks[bk], sf::Color(0,0,255,100), window);
	}
	if (drawOptions & drawAttW) {
		colorSquares(chessBoard.whiteAtt, sf::Color(255, 0, 0, 200), window);
	}
	int pieceIndex = 0;
	for (const auto& type : chessBoard.pieces){
		auto mask = type;
		for_bits(pos, mask) {
			if (pieceIndex > 5) {
			    pieces[pieceIndex].setPosition(((63 - pos) % 8)*orig_size.y*scale.y, ((63 - pos) / 8)*orig_size.x*scale.x);
			}
			else {
				pieces[pieceIndex].setPosition(((63 - pos) % 8)*orig_size.y*scale.y, ((63 - pos) / 8)*orig_size.x*scale.x);
			}
			window.draw(pieces[pieceIndex]);
		}
		pieceIndex++;
	}
	float dx = 0;
	for (auto& msg : messages) {
		msg.first.setPosition(sf::Vector2f(605, 400 + dx));
		dx += 16.0f;
		window.draw(msg.first);
	}
	scoreGauge.render(window);
	guiArrow.render(window);
}

bool Gui::handleEvent(sf::Event& ev, sf::RenderWindow& window)
{
	// Handles window events
	// returns true if human has played a move
	Move user_GUI_Move; // Probably incorrect, since user makes mistakes
						// Only contains raw data (from and to-square), without castling rights or specific attack types
	piece pieceClicked = nullPiece;
	int selectedSquare = nullSquare;
	bool movePlayed = false;
	sf::Vector2i mouse;
	string assemble;
	switch (ev.type) {
	case sf::Event::MouseButtonPressed:
		mouse = sf::Mouse::getPosition(window);

		if (mouse.x > HEIGHT) break; // Clicked outside board

		selectedSquare = 63 - ((mouse.x / (HEIGHT / 8)) + 8 * (mouse.y / (HEIGHT / 8)));

		assemble = "Klick at (" + to_string(mouse.x) + ',' + to_string(mouse.y) + ") = ";
		assemble += squareNames[selectedSquare];

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
				user_GUI_Move.pieces = userInput.movePiece;
				if (!isUserMoveValid_completeMoveInfo(user_GUI_Move)) {
					cerr << "Invalid move!\n";
					showMessage("Invalid move!", sf::Color::White);
				}
				else {
					chessBoard.makeMove<PROPER>(user_GUI_Move, humanColor);
					chessBoard.updateAllAttacks();
					lastMove = user_GUI_Move;

					//chessBoard.print();
					movePlayed = true;
				}
				userInput.reset();
			}
		}
		else { // Clicked on piece
			if (userInput.pieceSelected) { // May be a capture
				user_GUI_Move.from   = userInput.from;
				user_GUI_Move.to     = selectedSquare;
				user_GUI_Move.flags  = CAPTURE; // some kind of capture
				user_GUI_Move.pieces = piece_pair(userInput.movePiece, pieceClicked);
				if (!isUserMoveValid_completeMoveInfo(user_GUI_Move)) {
					cerr << "Invalid move!\n";
					showMessage("Invalid move!", sf::Color::White);
				}
				else {
					chessBoard.makeMove<PROPER>(user_GUI_Move, humanColor);
					chessBoard.updateAllAttacks();
					lastMove = user_GUI_Move;
					movePlayed = true;
				}
				userInput.reset();
			}
			else { // Nothing. New piece was selected
				userInput.pieceSelected = true;
				userInput.movePiece = pieceClicked;
				userInput.from = selectedSquare;
			}
		}
		break;
	case sf::Event::KeyPressed:
		switch (ev.key.code) {
		case sf::Keyboard::F1:
			drawOptions = ((drawOptions & drawAttB) ? (drawOptions & ~drawAttB) : (drawOptions | drawAttB));
			break;
		case sf::Keyboard::F2:
			drawOptions = ((drawOptions & drawAttW) ? (drawOptions & ~drawAttW) : (drawOptions | drawAttW));
			break;
		}
		break;
	}
	return movePlayed;
}

bool Gui::isUserMoveValid_completeMoveInfo(Move& inputMove)
{
	// Checks if selected move can be played. If it can,
	// the needed move metadata is filled in correctly.
	// -> Generates all possible moves and tries to find a match.

	MoveList possibleMoves;
	chessBoard.generateMoveList(possibleMoves, humanColor, true);

	MoveList::iterator matchingMove = find_if(possibleMoves.begin(), possibleMoves.end(), [&](const Move& pmove) {
		byte pflags = pmove.flags & 0xF;
		if (pflags == BCASTLE || pflags == WCASTLE || pflags == BCASTLE_2 || pflags == WCASTLE_2){
			// Check if human wants to castle
			if (userInput.movePiece == bk && humanColor == black) {
				if (inputMove.from - inputMove.to ==  2 && chessBoard.castlingRights & castle_k && pflags == BCASTLE)   return true;
				if (inputMove.from - inputMove.to == -2 && chessBoard.castlingRights & castle_q && pflags == BCASTLE_2) return true;
			}
			else if (userInput.movePiece == wk && humanColor == white) {
				if (inputMove.from - inputMove.to ==  2 && chessBoard.castlingRights & castle_K && pflags == WCASTLE)   return true;
				if (inputMove.from - inputMove.to == -2 && chessBoard.castlingRights & castle_Q && pflags == WCASTLE_2) return true;
			}
		}
		else if (pmove.from == inputMove.from && pmove.to == inputMove.to) {
			// Strong indicator for existent move
			if (pflags == MOVE && inputMove.flags == MOVE) {
				// quiet move is allowed
				return true;
			}
			else if (pflags == CAPTURE && inputMove.flags == CAPTURE) {
				return true;
			}
			else if (inputMove.flags == MOVE && (pmove.targetPiece == bq || pmove.targetPiece == wq)) {
				// Promotion ?
				if (pflags & PROMOTION) {
					return true;
				}
			}
			else if (inputMove.flags == CAPTURE && (pmove.targetPiece == bq || pmove.targetPiece == wq)) {
				// Promotion ?
				if (pflags & C_PROMOTION) {
					return true;
				}
			}
			else {
				if (inputMove.pieces == pmove.pieces) {
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
	else { // Check if move is legal
		inputMove = *matchingMove;
		chessBoard.makeMove<PROPER>(inputMove, humanColor);
		chessBoard.updateAllAttacks();
		if ((humanColor == black && chessBoard.isKingInCheck(black)) ||
			(humanColor == white && chessBoard.isKingInCheck(white))) {
			chessBoard.unMakeMove<PROPER>(inputMove, humanColor);
			chessBoard.updateAllAttacks();
			return false;
		}
		chessBoard.unMakeMove<PROPER>(inputMove, humanColor);
		chessBoard.updateAllAttacks();
		return true;
	}
}

void Gui::UserInput::reset()
{
	from = nullSquare;
	pieceSelected = false;
	movePiece = nullPiece;
}

void Gui::colorSquares(U64 pattern, sf::Color color, sf::RenderWindow& window)
{
	// Colors specific squares
	sf::RectangleShape colorRect(sf::Vector2f(HEIGHT / 8, HEIGHT / 8));
	colorRect.setFillColor(color);
	for_bits(pos, pattern) {
		colorRect.setPosition(((-(int)pos + 63) % 8)*HEIGHT / 8, ((-(int)pos + 63) / 8)*HEIGHT / 8);
		window.draw(colorRect);
	}
}

void Gui::debugDrawSquareNumering(sf::RenderWindow& window)
{
	// Draws bit-numbers on squares
	sf::Text Sq_Number("?", textFont);
	Sq_Number.setFillColor(sf::Color(0, 0, 0, 20));
	Sq_Number.setCharacterSize(60);
	for (int i = 0; i < 64; i++) {
		Sq_Number.setString(to_string(i));
		Sq_Number.setPosition(sf::Vector2f(((63 - i) % 8) * size, ((63 - i) / 8) * size));
		window.draw(Sq_Number);
	}
}

void Gui::visualizeScore(int ChessScore)
{
	scoreGauge.update(ChessScore, humanColor == black ? white : black);
}

void Gui::visualizeLastMove(const Move& move)
{
	guiArrow.setMove(move);
}

void Gui::showMessage(const string& msg, sf::Color color=sf::Color::White)
{
	if (messages.size() == 5)
		messages.erase(messages.begin());
	messages.push_back(pair<sf::Text, float>(sf::Text(msg, textFont, 15), 0.0f));
	messages.back().first.setFillColor(color);
}

Gui::ScoreGauge::ScoreGauge()
{
	if (!font.loadFromFile("FiraMono-Regular.otf")) {
		cerr << "Font File not found!\nAborting" << endl;
		cin.ignore();
		exit(1);
	}
	// Gauge position = origin
	origin = sf::Vector2f(700, 600);
	// Init Text
	scoreText.setFont(font);
	scoreText.setString("0");
	scoreText.setPosition(origin.x-30, origin.y-30);
	scoreText.setCharacterSize(20);
	scoreText.setFillColor(sf::Color::White);
	// Init black circle
	midCircle = sf::CircleShape(50, 30);
	midCircle.setFillColor(sf::Color::Black);
	midCircle.setOrigin(50, 50);
	midCircle.setPosition(origin);
	// Init gauge
	whiteSide.setPointCount(32);
	blackSide.setPointCount(32);
	whiteSide.setOrigin(origin);
	blackSide.setOrigin(origin);
	whiteSide.setPosition(origin);
	blackSide.setPosition(origin);
	whiteSide.setPoint(0, origin);
	blackSide.setPoint(0, origin);

	whiteSide.setFillColor(sf::Color::White);
	blackSide.setFillColor(sf::Color(60, 60, 60));

	int c = 1;
	for (float alpha = 0; alpha <= M_PI; alpha += M_PI / 30) {
		whiteSide.setPoint(c, sf::Vector2f(90  * cos(alpha + M_PI_2) + origin.x, -90 * sin(alpha + M_PI_2) + origin.y));
		blackSide.setPoint(c, sf::Vector2f(-90 * cos(alpha + M_PI_2) + origin.x, -90 * sin(alpha + M_PI_2) + origin.y));
		c++;
	}
}

void Gui::ScoreGauge::update(int ChessScore, color side)
{
	scoreText.setString(to_string(-ChessScore));
	scoreText.setFillColor(abs(ChessScore) >= 1000 ? sf::Color::Red : sf::Color::White);
	float angle = 0;
	if (abs(ChessScore) == oo) {
		if (ChessScore < 0) {
			angle = 90;
			scoreText.setString("-inf");
		}
		else {
			angle = -90;
			scoreText.setString("inf");
		}
	}
	else {
		angle = (atan((float)ChessScore / 1000.0f))/M_PI*180;
		if (ChessScore < 0) angle *= -1;
		if (side == black)  angle *= -1;
	}
	whiteSide.setRotation(angle);
	blackSide.setRotation(angle);
}

void Gui::ScoreGauge::render(sf::RenderWindow& window)
{
	window.draw(whiteSide);
	window.draw(blackSide);
	window.draw(midCircle);
	window.draw(scoreText);
	//window.draw(hide);
}

Gui::Arrow::Arrow()
{
	elapsedTime = 0;
	arrowBody.setFillColor(sf::Color(0, 0, 255, 200));
	arrowHead.setFillColor(sf::Color(0, 0, 255, 200));
	fadelevel = 200; // Fades out
}

void Gui::Arrow::setMove(const Move& move)
{
	elapsedTime = 0;
	fadelevel = 200;
	byte f = move.flags & 0xF;
	if (!(f & BCASTLE || f & WCASTLE
		|| f & BCASTLE_2 || f & WCASTLE_2)) {
	sf::Vector2f from = sf::Vector2f(7.0 - move.from % 8, 8.0 - move.from / 8.0) * (HEIGHT / 8.0f);
	sf::Vector2f   to = sf::Vector2f(7.0 - move.to   % 8, 8.0 - move.to   / 8.0) * (HEIGHT / 8.0f);
	sf::Vector2f connect(to.x - from.x, to.y - from.y);
	float distance = sqrt(pow(connect.x, 2) + pow(connect.y, 2));
	float angle = atan(connect.y / connect.x);
	arrowBody.setSize(sf::Vector2f(distance, 10));
	arrowBody.setOrigin(distance / 2, 5);
	arrowBody.setPosition(from + sf::Vector2f(connect.x / 2 + HEIGHT / 16, connect.y / 2));
	arrowBody.setRotation(angle / M_PI * 180);
	}
}

void Gui::Arrow::update(float frameTime)
{
	elapsedTime += frameTime;
	fadelevel = 200 - (elapsedTime / 5.0f) * 200; // Fades out after 5 seconds
	arrowBody.setFillColor(sf::Color(0, 0, 255, fadelevel));
	arrowHead.setFillColor(sf::Color(0, 0, 255, fadelevel));
}

void Gui::Arrow::render(sf::RenderWindow& window)
{
	if (fadelevel > 0) {
		window.draw(arrowBody);
	}
}

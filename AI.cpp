#include "AI.h"

AI::AI(string FEN, color computerColor)
	: aiColor(computerColor)
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation
	chessBoard = Board(FEN);
	transposition_hash = ZobristHash(static_cast<size_t>(1e7));
	//debug();
}

AI::AI(string FEN, color computerColor, uint hashSize)
	: aiColor(computerColor)
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation
	chessBoard = Board(FEN);
	transposition_hash = ZobristHash(hashSize);
	//debug();
}

void AI::printBoard()
{
	chessBoard.print();
}

void AI::bindGui(Gui* guiPtr)
{
	gui = guiPtr;
}

string AI::boardToString() const
{
	string board = string(64, '_');
	int c = 0;
	for (auto p : chessBoard.pieces) {
		for_bits(pos, p) {
			board[63 - pos] = names[c];
		}
		c++;
	}
	return board;
}

void AI::writeToHistory(const Move& move)
{
	gameHistory.push_back(pair<string, Move>(boardToString(), move));
}

void AI::printDebug(string showPieces)
{
	chessBoard.print();
	for (auto p : showPieces){
		auto drawPiece = getPieceIndex(p);
		///printBitboard(chessBoard.pieces[drawPiece]);
		///printBitboard(chessBoard.attacks[drawPiece]);
		if (chessBoard.pieces[drawPiece])
			printBitboardFigAttack(chessBoard.pieces[drawPiece], chessBoard.attacks[drawPiece], p);
	}
}

void AI::Play(sf::RenderWindow& window)
{

}

/*
void AI::sortMoves(MoveList& list)
{

}

pair<Move, int> AI::distributeNegaMax()
{
	
}

int AI::NegaMax_Search(nodePtr& node, int alpha, int beta, int depth, color side)
{

}
*/

const Board& AI::getBoardRef()
{
	return chessBoard;
}

Board* AI::getBoardPtr()
{
	return &chessBoard;
	//return static_cast<const Board*>(&chessBoard);
}

void AI::resetGame()
{
	gameHistory.clear();
}

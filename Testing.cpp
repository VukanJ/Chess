#include "Testing.h"

ChessError::ChessError(string msg) : errMsg(msg){}

void ChessError::what() const
{
	cerr << errMsg << '\n';
}

IntrinError::IntrinError(string msg, string name) : fname(name), ChessError(msg) {}

void IntrinError::what() const
{
	printf("Error in %s intrinsic\n", fname.c_str());
	cerr << errMsg << '\n';
}

UnitTest::UnitTest() {}

void UnitTest::testDefines() const
{
  byte b = piece_pair(0xA, 0x5);
  if (move_piece(b) != 0xA)
    cerr << "TARGET_PIECE() Failed\n";
  if (target_piece(b) != 0x5)
    cerr << "MOV_PIECE() Failed()\n";
}

void UnitTest::testIntrinsics() const
{
	// Intrinsics:
	/*
		POPCOUNT
		BITSCANR64
		ROTL64
		ROTR64
		BITLOOP
	*/
	const U64 randomCheckNum = 0xc69a3d1858e52a13;
	const U64 rotatedR[65] = {
		0xc69a3d1858e52a13, 0xe34d1e8c2c729509, 0xf1a68f4616394a84,
		0x78d347a30b1ca542, 0x3c69a3d1858e52a1, 0x9e34d1e8c2c72950,
		0x4f1a68f4616394a8, 0x278d347a30b1ca54, 0x13c69a3d1858e52a,
		0x09e34d1e8c2c7295, 0x84f1a68f4616394a, 0x4278d347a30b1ca5,
		0xa13c69a3d1858e52, 0x509e34d1e8c2c729, 0xa84f1a68f4616394,
		0x54278d347a30b1ca, 0x2a13c69a3d1858e5, 0x9509e34d1e8c2c72,
		0x4a84f1a68f461639, 0xa54278d347a30b1c, 0x52a13c69a3d1858e,
		0x29509e34d1e8c2c7, 0x94a84f1a68f46163, 0xca54278d347a30b1,
		0xe52a13c69a3d1858, 0x729509e34d1e8c2c, 0x394a84f1a68f4616,
		0x1ca54278d347a30b, 0x8e52a13c69a3d185, 0xc729509e34d1e8c2,
		0x6394a84f1a68f461, 0xb1ca54278d347a30, 0x58e52a13c69a3d18,
		0x2c729509e34d1e8c, 0x16394a84f1a68f46, 0x0b1ca54278d347a3,
		0x858e52a13c69a3d1, 0xc2c729509e34d1e8, 0x616394a84f1a68f4,
		0x30b1ca54278d347a, 0x1858e52a13c69a3d, 0x8c2c729509e34d1e,
		0x4616394a84f1a68f, 0xa30b1ca54278d347, 0xd1858e52a13c69a3,
		0xe8c2c729509e34d1, 0xf4616394a84f1a68, 0x7a30b1ca54278d34,
		0x3d1858e52a13c69a, 0x1e8c2c729509e34d, 0x8f4616394a84f1a6,
		0x47a30b1ca54278d3, 0xa3d1858e52a13c69, 0xd1e8c2c729509e34,
		0x68f4616394a84f1a, 0x347a30b1ca54278d, 0x9a3d1858e52a13c6,
		0x4d1e8c2c729509e3, 0xa68f4616394a84f1, 0xd347a30b1ca54278,
		0x69a3d1858e52a13c, 0x34d1e8c2c729509e, 0x1a68f4616394a84f,
		0x8d347a30b1ca5427, 0xc69a3d1858e52a13
	};
	const int indices[] = {
		63, 62, 58, 57, 55,
		52, 51, 49, 45, 44,
		43, 42, 40, 36, 35,
		30, 28, 27, 23, 22,
		21, 18, 16, 13, 11,
		9, 4, 1, 0
	};

	ulong index;
	clog << string(8, '~') << "::: Testing intrinsics :::" << string(8, '~') << '\n';
	int i = 0;
	assert(popcount(static_cast<U64>(0x0)) == 0);
	for (U64 p = 0x1; i < 64; p |= p << 1, ++i)
		assert(popcount(p) == i + 1);
	assert(popcount(0xFFull << 56) == 8); // Check if popcnt is really 64 bit
	clog << "bitscan...\n";
	bitScan_rev64(index, 0x1ull << 10); // counts from least significant bit
	assert(index == 10);
	bitScan_rev64(index, 0x0ull); // 0 if 0 (!)
	assert(index == 0);
	bitScan_rev64(index, 0x1ull); // 0 if 1 (!)
	assert(index == 0);
	bitScan_rev64(index, 0x1ull << 63); // 63 at msb
	assert(index == 63);
	clog << "rotr...\n";

	U64 testNum = randomCheckNum;
	for (int i = 0; i < 64; ++i) {
		testNum = rotate_r64(testNum, 1);
		//cout << hex << rotatedR[i+1] << ' ' << testNum << endl;
		assert(testNum == rotatedR[i+1]);
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 16; ++i) {
		testNum = rotate_r64(testNum, 4);
		assert(testNum == rotatedR[(i + 1) * 4]);
		//cout << hex << testNum << ' ' << randomCheckNum << endl;
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 8; ++i) {
		testNum = rotate_r64(testNum, 8);
		assert(testNum == rotatedR[(i + 1) * 8]);
		//cout << hex << testNum << ' ' << randomCheckNum << endl;
	}
	assert(testNum == randomCheckNum);
	testNum = rotate_r64(testNum, 70);
	testNum = rotate_r64(testNum, 58);
	assert(testNum == randomCheckNum);

	clog << "rotl...\n";

	for (int i = 0; i < 64; ++i) {
		testNum = rotate_l64(testNum, 1);
		//cout << hex << rotatedR[i+1] << ' ' << testNum << endl;
		assert(testNum == rotatedR[64 - i - 1]);
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 16; ++i) {
		testNum = rotate_l64(testNum, 4);
		//cout << hex << rotatedR[60 - (i * 4)] << ' ' << testNum << endl;
		assert(testNum == rotatedR[60 - (i * 4)]);
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 8; ++i) {
		testNum = rotate_l64(testNum, 8);
		assert(testNum == rotatedR[56 - (i * 8)]);
		//cout << hex << testNum << ' ' << randomCheckNum << endl;
	}
	assert(testNum == randomCheckNum);
	testNum = rotate_l64(testNum, 70);
	testNum = rotate_l64(testNum, 58);
	assert(testNum == randomCheckNum);
	clog << "bitloop...\n";
	int j = 0;
	BITLOOP(index, testNum) {
		assert(index == indices[j++]);
		//printBitboard(testNum);
		//printBitboard((randomCheckNum & ~(ULLONG_MAX << index) | BIT_AT(index)));
		assert(testNum == (randomCheckNum & ~(ULLONG_MAX << index) | bit_at(index)));
		//printBitboard(testNum);
	}
	cout << "Testing Bitloops" << endl;
	auto mask = 0xFFFFFFFFFFFFFFFF;
	j = 63;
	BITLOOP(pos, mask) {
		assert(pos == j--);
	}
	assert(j == -1);
	cout << "Intrinsics passed all tests!\n";
}

void UnitTest::testGenerationAlgorithms()
{
	clog << string(8,'~') << "::: Testing moveGenerator :::" << string(8, '~') << '\n';
	cout << "Testing Board::pawnfill()...\n";
	//testPawnFill();
	cout << "Testing castling...\n";
	testCastling();
	cout << "Testing pawn promotion...\n";
	//testProm();
	cout << "Testing enpassent...\n";
	testEnpassent();
}

void UnitTest::testPawnFill()
{
	AI ai("8/7p/P7/2P5/8/1p4p1/P2PP2P/8 w - - 1 0", black, 10);
	assert(ai.chessBoard.attacks[wp] == 0x80200099db0000ull);
	assert(ai.chessBoard.attacks[bp] == 0x1010000c300ull);
	ai.chessBoard.setupBoard("8/p2pp2p/1P4P1/8/8/pp4p1/7P/8 w - - 1 0");
	assert(ai.chessBoard.attacks[wp] == 0xc3000001030000);
	assert(ai.chessBoard.attacks[bp] == 0xdb990000c300);

	// Is pawn capture oriented correctly

	ai.chessBoard.setupBoard("8/4p3/3N1N2/88888 w - - 1 0");
	auto hashKey = ai.chessBoard.hashKey;
	vector<Move> moveList;
	ai.chessBoard.generateMoveList(moveList, black, true);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == CAPTURE && move.from > move.to; }) == 2);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == MOVE && move.from > move.to; }) == 1);
	for (auto& m : moveList) {
		ai.chessBoard.makeMove(m, black);
		ai.chessBoard.unMakeMove(m, black);
	}
	assert(hashKey == ai.chessBoard.hashKey);

	ai.chessBoard.setupBoard("8/8/3n1n2/4P3/8888 w - - 1 0");
	hashKey = ai.chessBoard.hashKey;
	moveList.clear();
	ai.chessBoard.generateMoveList(moveList, white, true);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == CAPTURE && move.from < move.to; }) == 2);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == MOVE && move.from < move.to; }) == 1);
	for (auto& m : moveList) {
		ai.chessBoard.makeMove(m, white);
		ai.chessBoard.unMakeMove(m, white);
	}
	assert(hashKey == ai.chessBoard.hashKey);

}

void UnitTest::testCastling()
{
	auto getMove = [](vector<Move>& mlist, int f, int t) {
		return find_if(mlist.begin(), mlist.end(),
			[f, t](const Move& m) {return m.from == f && m.to == t; }); };

	vector<Move> moveList;
	AI ai("r3k2r/8/8/8/8/8/8/R3K2R w - - 1 0", black, 10);
	ai.chessBoard.generateMoveList(moveList, black, true);

	assert(!any_of(moveList.begin(), moveList.end(), [](Move& move) {
		return move.flags == BCASTLE || move.flags == BCASTLE_2; }));

	ai.chessBoard.setupBoard("r3k2r/8/8/8/8/8/8/R3K2R w KkQq - 1 0");

	moveList.clear();
	ai.chessBoard.generateMoveList(moveList, black, true);
	auto hashKey = ai.chessBoard.hashKey;
	assert(find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == BCASTLE;   }) != moveList.end()
		&& find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == BCASTLE_2; }) != moveList.end());
	Move boo(ai.chessBoard.castlingRights, BCASTLE);
	Move bOO(ai.chessBoard.castlingRights, BCASTLE_2);
	ai.chessBoard.makeMove(boo,   black);
	assert(ai.chessBoard.castlingRights == 0b1100);
	ai.chessBoard.unMakeMove(boo, black);
	assert(ai.chessBoard.castlingRights == 0b1111);
	ai.chessBoard.makeMove(bOO,   black);
	assert(ai.chessBoard.castlingRights == 0b1100);
	ai.chessBoard.unMakeMove(bOO, black);
	assert(ai.chessBoard.castlingRights == 0b1111);
	assert(ai.chessBoard.hashKey == hashKey);

	moveList.clear();
	ai.chessBoard.generateMoveList(moveList, white, true);
	hashKey = ai.chessBoard.hashKey;
	assert(find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == WCASTLE;   }) != moveList.end()
		&& find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == WCASTLE_2; }) != moveList.end());
	Move woo(ai.chessBoard.castlingRights, WCASTLE);
	Move wOO(ai.chessBoard.castlingRights, WCASTLE_2);
	ai.chessBoard.makeMove(woo,   white);
	assert(ai.chessBoard.castlingRights == 0b0011);
	ai.chessBoard.unMakeMove(woo, white);
	assert(ai.chessBoard.castlingRights == 0b1111);
	ai.chessBoard.makeMove(wOO,   white);
	assert(ai.chessBoard.castlingRights == 0b0011);
	ai.chessBoard.unMakeMove(wOO, white);
	assert(ai.chessBoard.castlingRights == 0b1111);
	assert(ai.chessBoard.hashKey == hashKey);
	// Check partial loss of castling rights

	Move Rook1(a1, a2, move_metadata(MOVE, castle_Q), wr);
	Move Rook2(h1, h2, move_metadata(MOVE, castle_K), wr);
	Move rook1(a8, a7, move_metadata(MOVE, castle_q), br);
	Move rook2(h8, h7, move_metadata(MOVE, castle_k), br);

	hashKey = ai.chessBoard.hashKey;

	ai.chessBoard.makeMove(Rook1, white);
	ai.chessBoard.unMakeMove(Rook1, white);
	ai.chessBoard.makeMove(Rook2, white);
	ai.chessBoard.unMakeMove(Rook2, white);
	assert(hashKey == ai.chessBoard.hashKey);

	ai.chessBoard.makeMove(rook1, black);
	ai.chessBoard.unMakeMove(rook1, black);
	ai.chessBoard.makeMove(rook2, black);
	ai.chessBoard.unMakeMove(rook2, black);
	assert(hashKey == ai.chessBoard.hashKey);

	ai.chessBoard.setupBoard("8/8/8/8/8/8/7r/R3K2R w KQ - 1 0");
	hashKey = ai.chessBoard.hashKey;
	// Metadata should be filled in by MoveGenerator
	Move captureRook2(h2, h1, move_metadata(CAPTURE, castle_K), wr);
	Move captureRook1(a2, a1, move_metadata(CAPTURE, castle_Q), wr);
	ai.chessBoard.makeMove(captureRook1,   black);
	assert(ai.chessBoard.castlingRights == 0b0100);
	ai.chessBoard.unMakeMove(captureRook1, black);

	ai.chessBoard.makeMove(captureRook2,   black);
	ai.chessBoard.unMakeMove(captureRook2, black);
	assert(hashKey == ai.chessBoard.hashKey);
	// Rook moves back and forth (Lead to incorrect castling right update in the past)

	moveList.clear();
	ai.chessBoard.setupBoard("r3k2r/pppppppp/8888/PPPPPPPP/R3K2R w KkQq - 1 0");
	ai.chessBoard.generateMoveList(moveList, white, true);
	assert(ai.chessBoard.castlingRights == 0b1111);
	auto rookMove = getMove(moveList, 0, 1);
	assert(rookMove != moveList.end());
	ai.chessBoard.makeMove(*rookMove, white);
	ai.chessBoard.updateAllAttacks();
	ai.chessBoard.print();
	printBits(ai.chessBoard.castlingRights);
	assert(ai.chessBoard.castlingRights == 0b1011);

	moveList.clear();
	ai.chessBoard.generateMoveList(moveList, white, true);
	assert(ai.chessBoard.castlingRights == 0b1011);
	rookMove = getMove(moveList, 1, 0);
	assert(rookMove != moveList.end());
	ai.chessBoard.makeMove(*rookMove, white);
	ai.chessBoard.updateAllAttacks();
	ai.chessBoard.print();
	printBits(ai.chessBoard.castlingRights);
	assert(ai.chessBoard.castlingRights == 0b1011);
}

void UnitTest::testEnpassent()
{
	// Test enpassent move system
	AI ai("4k3/pppppppp8888PPPPPPPP/4K3 w - - 1 0", white, 10);
	MoveList movelist;
	ai.chessBoard.updateAllAttacks();

	auto hashKey = ai.chessBoard.hashKey;

	// Test all possible enpassent moves for black
	Move pawn2;            // pawn that is captured
	Move otherpawn;        // pawn, that performs enpassent
	MoveList::iterator ep; // enpassent
	for (int pos = 1; pos < 8; ++pos) {
		otherpawn = Move(h7 + pos, h4 + pos, MOVE, bp);
		pawn2 = Move(7 + pos, 23 + pos, PAWN2, wp);

		ai.chessBoard.makeMove(otherpawn, black);
		ai.chessBoard.makeMove(pawn2, white);
		ai.chessBoard.updateAllAttacks();

		movelist.clear();
		ai.chessBoard.generateMoveList(movelist, black, true);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSENT; });
		assert(ep != movelist.end());
		assert(ep->from == h4 + pos && ep->to == 15 + pos && ep->pieces == bp);
		ai.chessBoard.makeMove(*ep, black);
		ai.chessBoard.unMakeMove(*ep, black);

		ai.chessBoard.unMakeMove(pawn2, white);
		ai.chessBoard.unMakeMove(otherpawn, black);
		ai.chessBoard.updateAllAttacks();

		assert(hashKey == ai.chessBoard.hashKey);
	}
	for (int pos = 0; pos < 7; ++pos) {
		otherpawn = Move(h7 + pos, h4 + pos, MOVE, bp);
		pawn2 = Move(9 + pos, 25 + pos, PAWN2, wp);

		ai.chessBoard.makeMove(otherpawn, black);
		ai.chessBoard.makeMove(pawn2, white);
		ai.chessBoard.updateAllAttacks();

		movelist.clear();
		ai.chessBoard.generateMoveList(movelist, black, true);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSENT; });
		assert(ep != movelist.end());
		assert(ep->from == h4 + pos && ep->to == 17+pos && ep->pieces == bp);

		ai.chessBoard.makeMove(*ep, black);
		ai.chessBoard.unMakeMove(*ep, black);

		ai.chessBoard.unMakeMove(pawn2, white);
		ai.chessBoard.unMakeMove(otherpawn, black);
		ai.chessBoard.updateAllAttacks();

		assert(hashKey == ai.chessBoard.hashKey);
	}
	ai.chessBoard.updateAllAttacks();
	for (int pos = 0; pos < 7; ++pos) {
		otherpawn = Move(h2 + pos, h5 + pos, MOVE, wp);
		pawn2 = Move(g7 + pos, g5 + pos, PAWN2, bp);

		ai.chessBoard.makeMove(otherpawn, white);
		ai.chessBoard.makeMove(pawn2, black);
		ai.chessBoard.updateAllAttacks();

		movelist.clear();
		ai.chessBoard.generateMoveList(movelist, white, true);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSENT; });
		assert(ep != movelist.end());
		assert(ep->from == h5 + pos && ep->to == g6 + pos && ep->pieces == wp);
		ai.chessBoard.makeMove(*ep,   white);
		ai.chessBoard.unMakeMove(*ep, white);

		ai.chessBoard.unMakeMove(pawn2, black);
		ai.chessBoard.unMakeMove(otherpawn, white);
		ai.chessBoard.updateAllAttacks();

		assert(hashKey == ai.chessBoard.hashKey);
	}
	for (int pos = 1; pos < 8; ++pos) {
		otherpawn = Move(h2 + pos, h5 + pos, MOVE, wp);
		pawn2 = Move(h7 - 1 + pos, h5 - 1 + pos, PAWN2, bp);

		ai.chessBoard.makeMove(otherpawn, white);
		ai.chessBoard.makeMove(pawn2, black);
		ai.chessBoard.updateAllAttacks();

		movelist.clear();
		ai.chessBoard.generateMoveList(movelist, white, true);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSENT; });
		assert(ep != movelist.end());
		assert(ep->from == h5 + pos && ep->to == a5 + pos && ep->pieces == wp);
		ai.chessBoard.makeMove(*ep,   white);
		ai.chessBoard.unMakeMove(*ep, white);

		ai.chessBoard.unMakeMove(pawn2, black);
		ai.chessBoard.unMakeMove(otherpawn, white);
		ai.chessBoard.updateAllAttacks();

		assert(hashKey == ai.chessBoard.hashKey);
	}
	ai.chessBoard.print();
	Move m1(b7, b5, PAWN2, bp);
	Move m2(d7, d5, PAWN2, bp);
	Move dummy(h2, h3, MOVE, wp);

	pawn2 = Move(c2, c5, MOVE, wp);
	ai.chessBoard.makeMove(pawn2, white);
	ai.chessBoard.print();
	cout << (int)ai.chessBoard.w_enpassent << endl;
	ai.chessBoard.makeMove(m1, black);
	ai.chessBoard.print();
	cout << (int)ai.chessBoard.w_enpassent << endl;
	ai.chessBoard.makeMove(dummy, white);
	ai.chessBoard.print();
	cout << (int)ai.chessBoard.w_enpassent << endl;
	ai.chessBoard.makeMove(m2, black);
	ai.chessBoard.print();
	cout << (int)ai.chessBoard.w_enpassent << endl;
	ai.chessBoard.unMakeMove(m2, black);
	ai.chessBoard.print();
	cout << (int)ai.chessBoard.w_enpassent << endl;
}

void UnitTest::testProm()
{
	AI ai("5n2/1P4P1/8/8/8/8/1p4p1/5N w - - 1 0", black, 10);
	ai.chessBoard.updateAllAttacks();
	ai.chessBoard.print();
	vector<Move> whiteMoves, blackMoves;
	ai.chessBoard.generateMoveList(whiteMoves, white, true);
	ai.chessBoard.generateMoveList(blackMoves, black, true);
	assert(count_if(whiteMoves.begin(), whiteMoves.end(), [](Move& move) {return move.flags == PROMOTION; })   == 8);
	assert(count_if(whiteMoves.begin(), whiteMoves.end(), [](Move& move) {return move.flags == C_PROMOTION; }) == 4);
	assert(count_if(blackMoves.begin(), blackMoves.end(), [](Move& move) {return move.flags == PROMOTION; })   == 8);
	assert(count_if(blackMoves.begin(), blackMoves.end(), [](Move& move) {return move.flags == C_PROMOTION; }) == 4);
	auto hashKey = ai.chessBoard.hashKey;
	for (auto& m : blackMoves) {
		ai.chessBoard.makeMove(m, black);
		ai.chessBoard.unMakeMove(m, black);
	}
	assert(hashKey == ai.chessBoard.hashKey);
	for (auto& m : whiteMoves) {
		ai.chessBoard.makeMove(m, white);
		ai.chessBoard.unMakeMove(m, white);
	}
	assert(hashKey == ai.chessBoard.hashKey);
}

void UnitTest::specialTest()
{
	AI ai("k7/8/3K4/8/8/6q1/8/5N2 w - - 1 0", white);
	
	ai.chessBoard.updateAllAttacks();
	ai.chessBoard.print();
	MoveList movelist;

	ai.chessBoard.generateMoveList(movelist, white, true);
	
	exit(0);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ MINIMAL TREE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

UnitTest::MinimalTree::MinimalTree(Board& _chessBoard, color comp, int _targetDepth)
	: Root(nullptr), targetDepth(_targetDepth), chessBoard(_chessBoard), computerColor(comp)
{
	//Root.reset(new Node(chessBoard.evaluate(true)));
	staticEvaluations = 0;
}

void UnitTest::testMinimalTree()
{
	//AI ai("2r3k1/1p2Bpp1/p3p1p1/bq1pP3/1n1P2PQ/1R3N2/1P3PKP/8 w - 1 0", white);
	AI ai("rnb2b1r/ppk2ppp/2p5/4q1B1/88/PPP2PPP/2KR1BNR w - 1 0", white);
	ai.chessBoard.print();
	MinimalTree tree(ai.chessBoard, white, 4);
	cout << tree.buildGameTreeMinimax(4, white) << endl;
}

UnitTest::MinimalTree::Node::Node(float _boardValue) : boardValue(_boardValue) {}

int UnitTest::MinimalTree::buildGameTreeMinimax(int depth, color side)
{
	// Even depths correspond to maximizing player (computer)
	bool isMax = side == computerColor;

	if (depth == 0) return chessBoard.evaluate(side);
	vector<Move> moveList;
	int bestValue = isMax ? -oo: +oo;
	int testValue;

	chessBoard.generateMoveList(moveList, side, true);
	static auto bestMove = moveList.front();

	for (auto move = moveList.begin(); move != moveList.end(); move++) {
		chessBoard.makeMove(*move, side);
		if (isMax) {
			testValue = buildGameTreeMinimax(depth - 1, side == black ? white : black);
			if (testValue > bestValue && depth == targetDepth) {
				bestMove = *move;
				cout << "New best move -> " << moveString(*move) << endl;
			}
			bestValue = max(bestValue, testValue);
		}
		else {
			testValue = buildGameTreeMinimax(depth - 1, side == black ? white : black);
			if (testValue < bestValue && depth == targetDepth) {
				bestMove = *move;
				cout << "New best move -> " << moveString(*move) << endl;
			}
			bestValue = min(bestValue, testValue);
		}
		chessBoard.unMakeMove(*move, side);
	}
	if (depth == targetDepth) cout << "Best Move ==> " << moveString(bestMove) << endl;
	return bestValue;
}

void UnitTest::testEvaluation()
{
	{
		AI ai("*", white);
		assert(ai.chessBoard.evaluate(black) == ai.chessBoard.evaluate(white)); // Symmetric position
	}
	AI ai("3q1rk1/pp4bp/3p1p2/3N1pB1/2r5/1N6/PPPQ3P/1K5R w - 1 0", white);
	//cout << "Score (w)" << ai.chessBoard.evaluate(white) << endl;
	ai.chessBoard.print();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ NEGAMAX TREE ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//

void UnitTest::testFullTree()
{
    // http://www.chesspuzzles.com/
	AI ai("rk5r/pp1Q1p1p/1q1p1N2/88/6P1/PP3PBP/2R3K1 w - 1 0", white); // Mate in 2 puzzle
	// Solution: Re8 Bf8 Bh6 d6 Rxf8#
	ai.chessBoard.print();
	auto hash = ai.chessBoard.hashKey;
	fullTree tree(ai.chessBoard, white, 6);
	cout << "Start with Enter\n";
	cin.ignore();
	tree.test_NegaMax(tree.Root, -oo, oo, 6, white);
	cout << "# Full Evaluations:   " << tree.staticEvaluations << endl;
	cout << "# Alpha Beta Cutoffs: " << tree.nalphaBeta << endl;
	cout << "# Hash Lookups:       " << tree.nHashLookups << endl;

	for (int n = 0; n < tree.Root->nodeList.size(); n++) {
		cout << moveString(tree.Root->nodeList[n]->thisMove) << "; V = " << tree.Root->nodeList[n]->thisValue << '\n';
	}

	// Check if hashKey is still consistent
	assert(hash == ai.chessBoard.hashKey);
}

UnitTest::fullTree::fullTree(Board& _chessBoard, color comp, int _targetDepth)
	: Root(nullptr), targetDepth(_targetDepth), chessBoard(_chessBoard), computerColor(comp)
{
	Root.reset(new Node()); // Computer plays at Root
	staticEvaluations = nalphaBeta = nHashLookups = 0;
}

UnitTest::fullTree::Node::Node() {}

int UnitTest::fullTree::test_NegaMax(unique_ptr<Node>& node, int alpha, int beta, int depth, color side)
{
	int bestValue = -oo, boardValue = 0;
	if (depth == 0) {
		//if (chessBoard.hash.hasBetterEntry(chessBoard.hashKey, targetDepth - depth)) {
		//	// Use pre-calculated value if it exists
		//	chessBoard.hash.getEntry(chessBoard.hashKey, boardValue);
		//	nHashLookups++;
		//}
		//else {
		//	// Else make new hash-entry and evaluate board
		//	boardValue = chessBoard.evaluate(side, targetDepth - depth);
		//	chessBoard.hash.addEntry(chessBoard.hashKey, boardValue, targetDepth - depth);
		//	staticEvaluations++;
		//}
		return boardValue;
	}
	chessBoard.generateMoveList(node->moveList, side, true);
	for (auto move = node->moveList.begin(); move != node->moveList.end();) {
		// Play move
		//cout << moveString(*move) << endl;
		chessBoard.makeMove(*move, side);
		// Is king in check?
		if (chessBoard.pieces[side == black ? bk : wk] & (side == black ? chessBoard.whiteAtt : chessBoard.blackAtt)) {
			chessBoard.unMakeMove(*move, side);
			move = node->moveList.erase(move);
			if (node->moveList.empty()) {
				//cout << "Mate in " << ceil((float)(targetDepth - depth) / 2.0) << " for " << (side==black ? "white" : "black") << endl;
				boardValue = oo; // Checkmate
			}
			continue;
		}
		node->nodeList.push_back(unique_ptr<Node>(new Node()));
		node->nodeList.back()->thisMove = *move;
		boardValue = -test_NegaMax(node->nodeList.back(), -beta, -alpha, depth - 1, side == black ? white : black);
		node->nodeList.back()->thisValue = boardValue;
		bestValue = max(bestValue, boardValue);
		alpha = max(alpha, boardValue);
		if (alpha >= beta) {
			nalphaBeta++;
			chessBoard.unMakeMove(*move, side);
			break;
		}
		chessBoard.unMakeMove(*move, side);
		move++;
	}
	if (depth == targetDepth) {
		cout << "bestValue: " << bestValue << endl;
	}
	return bestValue;
}

void UnitTest::testHashing()
{
	ZobristHash Hash(size_t(1e3));
	random_device r_device;
	mt19937_64 generator(r_device());
	uniform_int_distribution<U64> distr;

	auto boards = vector<U64>(100);
	for (auto& b : boards) {
		b = distr(generator);
		Hash.addEntry(b, (rand() % 1000) - 500, rand() % 6);
		//assert(Hash.hasEntry(b));
	}
	//assert(Hash.hasEntry(boards[55]));
}

void UnitTest::testMagic()
{
	/*
	cout << "Testing magic numbers...\n";
	U64 rook = bit_at(c3);
	U64 blocker = assembleBits({0});

	U64 mindex = blocker & rookAttackMasks[c3];
	mindex *= rookMagics[c3];
	mindex >>= rookMagicShifts[c3];
	printBitboard(magicRookMoveDatabase[c3][mindex]);

	// Start testing
	random_device r_device;
	mt19937_64 generator(r_device());
	generator.seed(time(0));
	uniform_int_distribution<U64> distr;

	Board board; // dummy board

	// Test rooks
	for (int i = 0; i < 1000000; ++i) {
		U64 randomBlockers = distr(generator) & distr(generator) & distr(generator);
		U64 rook = 0;

		int rookPos = rand() % popcount(~randomBlockers);
		int rookBoardPosition = 0;
		for (U64 pos = 1, c = -1; c != 64; pos <<= 1) {
			if (pos & (~randomBlockers)) {
				c++;
				if (c == rookPos) {
					rook = pos;
					rookBoardPosition = msb(pos);
					break;
				}
			}
		}
		if (rook & randomBlockers) {
			printBitboard(rook);
			printBitboard(randomBlockers);
			cout << "Generator failed\n";
		}

		mindex = randomBlockers & rookAttackMasks[rookBoardPosition];
		mindex = (mindex * rookMagics[rookBoardPosition]) >> rookMagicShifts[rookBoardPosition];
		U64 MagicAttacks = magicRookMoveDatabase[rookBoardPosition][mindex];

		U64 Truth = 0;
		Truth |= board.floodFill(rook, ~randomBlockers, Board::dir::n);
		Truth |= board.floodFill(rook, ~randomBlockers, Board::dir::e);
		Truth |= board.floodFill(rook, ~randomBlockers, Board::dir::s);
		Truth |= board.floodFill(rook, ~randomBlockers, Board::dir::w);
		if (Truth != MagicAttacks) {
			cout << "Magic failed! :(\n";
			cout << "Blockers = \n"; printBitboard(randomBlockers);
			cout << "Rook = \n"; printBitboard(rook);
			cout << "Truth = \n"; printBitboard(Truth);
			cout << "MagicAttacks = \n"; printBitboard(MagicAttacks);
		}
	}
	// Test Bishops
	for (int i = 0; i < 1000000; ++i) {
		U64 randomBlockers = distr(generator) & distr(generator) & distr(generator);
		U64 bish = 0;

		int bishPos = rand() % popcount(~randomBlockers);
		int bishBoardPosition = 0;
		for (U64 pos = 1, c = -1; c != 64; pos <<= 1) {
			if (pos & (~randomBlockers)) {
				c++;
				if (c == bishPos) {
					bish = pos;
					bishBoardPosition = msb(pos);
					break;
				}
			}
		}
		if (bish & randomBlockers) {
			printBitboard(bish);
			printBitboard(randomBlockers);
			cout << "Generator failed\n";
		}

		mindex = randomBlockers & bishopAttackMasks[bishBoardPosition];
		mindex = (mindex * bishopMagics[bishBoardPosition]) >> bishopMagicShifts[bishBoardPosition];
		U64 MagicAttacks = magicBishopMoveDatabase[bishBoardPosition][mindex];

		U64 Truth = 0;
		Truth |= board.floodFill(bish, ~randomBlockers, Board::dir::ne);
		Truth |= board.floodFill(bish, ~randomBlockers, Board::dir::se);
		Truth |= board.floodFill(bish, ~randomBlockers, Board::dir::sw);
		Truth |= board.floodFill(bish, ~randomBlockers, Board::dir::nw);
		if (Truth != MagicAttacks) {
			cout << "Magic failed! :(\n";
			cout << "Blockers = \n"; printBitboard(randomBlockers);
			cout << "Bishop = \n"; printBitboard(bish);
			cout << "Truth = \n"; printBitboard(Truth);
			cout << "MagicAttacks = \n"; printBitboard(MagicAttacks);
		}
	}
	*/
}

Benchmark::Benchmark() : performingAll(false)
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation

	testBoard = Board("1K1BQ3/2P3R1/P2P4/P3Pq1R/2n1p3/1p1r1p2/8/1kr5 w - - 1 0");
	
	//Move a8a4 = Move(a8, a4, MOVE, wr);
	//testBoard.makeMove(a8a4, white);
	// Now: 7r/8/8/2K5/R4k2/8/8/r6R b - - 1 0
	testBoard.print();

	testBoard.updateAllAttacks();
	MoveList moves;
	//testBoard.generateMoveList(moves, white, true);

	perftNodeCount = perftEPCount = perftMoveCount = perftCheckmateCount = totalPerftMoveCount = 0;
}

Benchmark::~Benchmark()
{
}

void Benchmark::performAllbenchmarks()
{
	clog << "\t::: STARTED ALL BENCHMARK :::\n";
	performingAll = true;
	benchmarkMoveGeneration();
	benchmarkMovemaking();
	performingAll = false;
	clog << "\t::: END OF ALL BENCHMARKS :::\n";
}

#pragma optimize( "", off ) // Never "optimize" benchmarks
void Benchmark::benchmarkMoveGeneration()
{
	if (performingAll) {
		results.push_back(result{ MOVEGEN, "generateMoveList", "", 0 });
	}
	else {
		clog << "\t::: BENCHMARK :::\n";
		clog << "Started Benchmarking Board::generateMoveList(...)\n";
	}
	// Measure move generation time
	AI samplePlayer("1K1BQ3/2P3R1/P2P4/P3Pq1R/2n1p3/1p1r1p2/8/1kr5 w kKqQ - 1 0", black);
	vector<Move> moves;

	int testSize = (int)1e7;
	moves.reserve(testSize);

	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
	for (int i = 0; i < testSize; i++) {
		samplePlayer.chessBoard.generateMoveList(moves, black, true);
		moves.clear();
	}
	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	auto deltaT_inSeconds = (double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count()*1e-6;
	
	cout << (long long) ((double)testSize*43.0 / deltaT_inSeconds) << " Boards per second\n";

	moves.clear();
	

}
#pragma optimize( "", on )

#pragma optimize( "", off ) // Never "optimize" benchmarks
void Benchmark::benchmarkMovemaking()
{
	// Measure move generation time
	if (performingAll) {
		results.push_back(result{ MAKEMOVE, "makeMove/unMakeMove", "", 0 });
	}
	else {
		clog << "\t::: BENCHMARK :::\n";
		clog << "Started Benchmarking Board::makeMove/unMakeMove(...)\n";
	}

	AI samplePlayer("1K1BQ3/2P3R1/P2P4/P3Pq1R/2n1p3/1p1r1p2/8/1kr5 w - - 1 0", black);
	vector<Move> moves;
	samplePlayer.chessBoard.generateMoveList(moves, black, true);
	auto& boardref = samplePlayer.chessBoard;
	size_t numOfMoves = moves.size();
	int testsize = (int)6e6;
	vector<double> measurement;
	measurement.reserve(testsize);

	for (int i = 0; i < testsize; i++) {
		chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
		for (auto& move : moves) {
			boardref.makeMove(move, black);
			boardref.unMakeMove(move, black);
		}
		chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
		measurement.push_back((double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count());
	}
	double sum = 0;
	for (auto& m : measurement) sum += m;
	double averageMoveMakeTime = (sum / (numOfMoves*testsize)) * 1e-6; // in seconds

	if (performingAll) {
		results.back().msg = "Number of million moves and unmake per second: ";
		results.back().value = averageMoveMakeTime;
	}
	else {
		printf("Move making and undoing takes approx. %f microseconds\n", averageMoveMakeTime * 1e6);
		printf("Makes %f move/unmakemoves per second\n", 1.0 / averageMoveMakeTime);
		clog << "\t::: END OF BENCHMARK :::\n";
	}
}
#pragma optimize( "", on )

void Benchmark::summarize()
{
	clog << "\t::: BENCHMARK SUMMARY:::\n" << string(80, '~') << '\n';
	for (auto& result : results) {
		clog << result.name << '\n';
		switch (result.type) {
		case MOVEGEN:
			clog << result.msg << '\t' << (1.0 / result.value) * 1e-6 << '\n';
			break;
		case MAKEMOVE:
			clog << result.msg << '\t' << (1.0 / result.value) * 1e-6 << '\n';
			break;
		}
	}
	clog << "\t::: END OF SUMMARY :::\n" << string(80, '~') << '\n';
}

void Benchmark::testPerft(int maxdepth)
{
	if (maxdepth == -1) {
		// Check perft numbers
		auto hashKey = testBoard.hashKey;
		for (int d = 1; d < 100; d++) {
			chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
			testBoard.print();
			perft(d, d, white);
			//testBoard.print();
			//printBitboard(testBoard.attacks[bk]);
			//printBitboard(testBoard.blackAtt);
			//testBoard.print();
			chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
			double microseconds = (double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
			cout << string(40, '=') << endl;
			cout << "Depth " << d << " summary:\n";
			cout << "\tExecution time " << microseconds * 1e-3 << "ms\n";
			cout << "\tComputed number of moves: " << totalPerftMoveCount << endl;
			cout << "\tE.P. count: " << perftEPCount << endl;
			cout << "\tNodes per second: " << (double)totalPerftMoveCount/microseconds << " M\n";

			totalPerftMoveCount = 0;
			perftEPCount = 0;
			assert(hashKey == testBoard.hashKey);
			cin.ignore();
		}
		return;
	}
	chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
	perft(maxdepth, maxdepth, white);
	chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
	auto microseconds = (double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count();

	cout << "Depth: " << maxdepth << endl;
	cout << "\tExecution time " << microseconds*1e-3 << "ms\n";
	cout << "\t# of generated nodes: " << perftNodeCount << endl;
	cout << "\t# of generated moves: " << perftMoveCount << endl;
	cout << "\t# of checkmates: " << perftCheckmateCount << endl;
	perftNodeCount = 0;
}

void Benchmark::perft(int depth, const int targetDepth, color side)
{
	// Builds a tree to benchmark move generation
	if (depth == 0) return;
	MoveList movelist;
	movelist.reserve(20);
	testBoard.updateAllAttacks();
	testBoard.generateMoveList(movelist, side, true);
	// Store pairs of root moves and number of possible moves after them
	bool checkmate = true;
	bool checkOnThisDepth = testBoard.wasInCheck;
	U64 pinnedOnThisDepth = testBoard.pinned;
	for (auto& move : movelist) {
		testBoard.makeMove(move, side);

		if(depth == 1) perftMoveCount++;
		if (testBoard.isKingLeftInCheck(side, move, checkOnThisDepth, pinnedOnThisDepth)) {
			testBoard.unMakeMove(move, side);
			if (depth == 1) perftMoveCount--;
			continue;
		}

		if (move.flags == ENPASSENT && depth == 1) perftEPCount++;

		checkmate = false; // Moves were found
		perft(depth - 1, targetDepth, static_cast<color>(!side));
		testBoard.unMakeMove(move, static_cast<color>(side));
		if (depth == targetDepth) {
			//cout << shortNotation(move) << ": " << perftMoveCount << endl;
			totalPerftMoveCount += perftMoveCount;
			totalTotalPerftMoveCount += perftMoveCount;
			perftMoveCount = 0;
		}
	}
	if (checkmate && depth == 1) perftCheckmateCount++;
}

// Scoreboard: AMD Phenom(tm) II X4 945
// commit 226506b6038d0991f0b10a4998adaea203f2af50
//		  Perft computation time: 85.6962 s (depth 5)
// commit 6e3edae2423852e0dd8c246825501630df8c1c2d:
//		  Perft computation time: 39.49 s (depth 5)
// commit 4abe35e1513ba6962cf6462e5192fe2e6c00817c:
//		  Perft computation time: ~31 s (depth 5)
// commit 7036cb32404d2365972fb87247fdf50589d0b8a1
//        Perft computation time: ~24.1 s (depth 5)
// commit 8db37125fadc7285670bc9f2a3f5b5c60da8fc0f
//        Perft computation time: ~18.75 s (depth 5)
// most recent commit
//        Perft computation time: ~15.60 s (depth 5)

void Benchmark::perftTestSuite()
{
	// Loading perft data and fens from file
	totalTotalPerftMoveCount = 0;
	ifstream testFile("perftsuite.txt", ios::in);
	if (!testFile.is_open()) {
		cout << "Error: Perft testsuite not found in project dir!\n";
		return;
	}
	vector<vector<string>> perftTestData = vector<vector<string>>(125, vector<string>(7, ""));
	string read;
	for (int i = 0; i < 125; ++i) {
		for (int j = 0; j < 12; ++j) {
			testFile >> read;
			if (read[0] != ';') {
				if(j < 6)
					perftTestData[i][0] += " " + read;
				else
					perftTestData[i][j-5] = read;
			}
			else j--;
		}
	}
	testFile.close();
	// Start perft
	int errorCount = 0;
	int c = 1;
	Timer timer;
	timer.start();
	for (auto& testNum : perftTestData) {
		c = 1;
		testBoard.setupBoard(testNum[0]);
		//testBoard.print();

		for (auto moveCount = testNum.begin() + 1; moveCount != testNum.end(); ++moveCount) {
			if (c == 6) continue;
			//cout << "Depth " << c << " start...\n";
			perft(c, c, any_of(testNum[0].begin(), testNum[0].end(), [](char c) {return c == 'w'; }) ? white : black);
			c++;
			if (totalPerftMoveCount != stol(*moveCount)) {
				cout << "Discrepancy detected: " << testNum[0] << " Depth : " << c - 1 << endl;
				cout << "Expected: " << *moveCount << " Found: " << totalPerftMoveCount << endl;
				cout << "Press Enter\n";
				errorCount++;
				//cin.ignore();
				totalPerftMoveCount = 0;
				break;
			}
			totalPerftMoveCount = 0;
		}
	}
	timer.stop();
	cout << "Correct percentage:       " << 100*((float)(perftTestData.size() - errorCount) / (float)perftTestData.size()) << "%\n";
	cout << "Computation time:         " << timer.getTime()*1e-6 << " s" << endl;
	cout << "Total moves examined:     " << totalTotalPerftMoveCount << endl;
	cout << "Average moves per second: " << ((float)totalTotalPerftMoveCount) / (timer.getTime()*1e-6)*1e-6 << " M\n";
	cin.ignore();
}

Timer::Timer()
{
	t1 = chrono::high_resolution_clock::now();
	t2 = t1;
}

void Timer::start()
{
	t1 = chrono::high_resolution_clock::now();
}

void Timer::stop()
{
	t2 = chrono::high_resolution_clock::now();
}

double Timer::getTime()
{
	return (double)chrono::duration_cast<chrono::microseconds>(t2 - t1).count();
}


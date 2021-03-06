﻿#include "Testing.h"

UnitTest::UnitTest() {}

void UnitTest::testDefines() const
{
  // obsolete
/*
  U8 b = piece_pair(0xA, 0x5);
  if (move_piece(b) != 0xA)
    cerr << "TARGET_PIECE() Failed\n";
  if (target_piece(b) != 0x5)
    cerr << "MOV_PIECE() Failed()\n";
	*/
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
	for_bits(index, testNum) {
		assert(index == indices[j++]);
		//printBitboard(testNum);
		//printBitboard((randomCheckNum & ~(ULLONG_MAX << index) | BIT_AT(index)));
		assert(testNum == (randomCheckNum & ~(ULLONG_MAX << index) | bit_at(index)));
		//printBitboard(testNum);
	}
	cout << "Testing Bitloops" << endl;
	auto mask = 0xFFFFFFFFFFFFFFFF;
	j = 63;
	for_bits(pos, mask) {
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
	cout << "Testing enpassant...\n";
	testEnpassent();
}

void UnitTest::testPawnFill()
{
	AI ai("8/7p/P7/2P5/8/1p4p1/P2PP2P/8 w - - 1 0", black, 10);
	assert(ai.board.attacks[wp] == 0x80200099db0000ull);
	assert(ai.board.attacks[bp] == 0x1010000c300ull);
	ai.board.setupBoard("8/p2pp2p/1P4P1/8/8/pp4p1/7P/8 w - - 1 0");
	assert(ai.board.attacks[wp] == 0xc3000001030000);
	assert(ai.board.attacks[bp] == 0xdb990000c300);

	// Is pawn capture oriented correctly

	ai.board.setupBoard("8/4p3/3N1N2/88888 w - - 1 0");
	auto hashKey = ai.board.hashKey;
	MoveList moveList;
	ai.board.generateMoveList<ALL>(moveList, black);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == CAPTURE && move.from > move.to; }) == 2);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == MOVE && move.from > move.to; }) == 1);
	for (auto& m : moveList) {
		ai.board.makeMove<FULL>(m, black);
		ai.board.unMakeMove<FULL>(m, black);
	}
	assert(hashKey == ai.board.hashKey);

	ai.board.setupBoard("8/8/3n1n2/4P3/8888 w - - 1 0");
	hashKey = ai.board.hashKey;
	moveList.clear();
	ai.board.generateMoveList<ALL>(moveList, white);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == CAPTURE && move.from < move.to; }) == 2);
	assert(count_if(moveList.begin(), moveList.end(), [](Move& move) {return move.flags == MOVE && move.from < move.to; }) == 1);
	for (auto& m : moveList) {
		ai.board.makeMove<FULL>(m, white);
		ai.board.unMakeMove<FULL>(m, white);
	}
	assert(hashKey == ai.board.hashKey);

}

void UnitTest::testCastling()
{
	auto getMove = [](MoveList& mlist, int f, int t) {
		return find_if(mlist.begin(), mlist.end(),
			[f, t](const Move& m) {return m.from == f && m.to == t; }); };

	MoveList moveList;
	AI ai("r3k2r/8/8/8/8/8/8/R3K2R w - - 1 0", black, 10);
	ai.board.generateMoveList<ALL>(moveList, black);

	assert(!any_of(moveList.begin(), moveList.end(), [](Move& move) {
		return move.flags == BCASTLE || move.flags == BCASTLE_2; }));

	ai.board.setupBoard("r3k2r/8/8/8/8/8/8/R3K2R w KkQq - 1 0");

	moveList.clear();
	ai.board.generateMoveList<ALL>(moveList, black);
	auto hashKey = ai.board.hashKey;
	assert(find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == BCASTLE;   }) != moveList.end()
		&& find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == BCASTLE_2; }) != moveList.end());
	Move boo(ai.board.castlingRights, BCASTLE);
	Move bOO(ai.board.castlingRights, BCASTLE_2);
	ai.board.makeMove<FULL>(boo,   black);
	assert(ai.board.castlingRights == 0b1100);
	ai.board.unMakeMove<FULL>(boo, black);
	assert(ai.board.castlingRights == 0b1111);
	ai.board.makeMove<FULL>(bOO,   black);
	assert(ai.board.castlingRights == 0b1100);
	ai.board.unMakeMove<FULL>(bOO, black);
	assert(ai.board.castlingRights == 0b1111);
	assert(ai.board.hashKey == hashKey);

	moveList.clear();
	ai.board.generateMoveList<ALL>(moveList, white);
	hashKey = ai.board.hashKey;
	assert(find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == WCASTLE;   }) != moveList.end()
		&& find_if(moveList.begin(), moveList.end(), [](Move& move) { return move.flags == WCASTLE_2; }) != moveList.end());
	Move woo(ai.board.castlingRights, WCASTLE);
	Move wOO(ai.board.castlingRights, WCASTLE_2);
	ai.board.makeMove<FULL>(woo,   white);
	assert(ai.board.castlingRights == 0b0011);
	ai.board.unMakeMove<FULL>(woo, white);
	assert(ai.board.castlingRights == 0b1111);
	ai.board.makeMove<FULL>(wOO,   white);
	assert(ai.board.castlingRights == 0b0011);
	ai.board.unMakeMove<FULL>(wOO, white);
	assert(ai.board.castlingRights == 0b1111);
	assert(ai.board.hashKey == hashKey);
	// Check partial loss of castling rights

	Move Rook1(a1, a2, move_metadata(MOVE, castle_Q), wr);
	Move Rook2(h1, h2, move_metadata(MOVE, castle_K), wr);
	Move rook1(a8, a7, move_metadata(MOVE, castle_q), br);
	Move rook2(h8, h7, move_metadata(MOVE, castle_k), br);

	hashKey = ai.board.hashKey;

	ai.board.makeMove<FULL>(Rook1, white);
	ai.board.unMakeMove<FULL>(Rook1, white);
	ai.board.makeMove<FULL>(Rook2, white);
	ai.board.unMakeMove<FULL>(Rook2, white);
	assert(hashKey == ai.board.hashKey);

	ai.board.makeMove<FULL>(rook1, black);
	ai.board.unMakeMove<FULL>(rook1, black);
	ai.board.makeMove<FULL>(rook2, black);
	ai.board.unMakeMove<FULL>(rook2, black);
	assert(hashKey == ai.board.hashKey);

	ai.board.setupBoard("8/8/8/8/8/8/7r/R3K2R w KQ - 1 0");
	hashKey = ai.board.hashKey;
	// Metadata should be filled in by MoveGenerator
	Move captureRook2(h2, h1, move_metadata(CAPTURE, castle_K), wr);
	Move captureRook1(a2, a1, move_metadata(CAPTURE, castle_Q), wr);
	ai.board.makeMove<FULL>(captureRook1,   black);
	assert(ai.board.castlingRights == 0b0100);
	ai.board.unMakeMove<FULL>(captureRook1, black);

	ai.board.makeMove<FULL>(captureRook2,   black);
	ai.board.unMakeMove<FULL>(captureRook2, black);
	assert(hashKey == ai.board.hashKey);
	// Rook moves back and forth (Lead to incorrect castling right update in the past)

	moveList.clear();
	ai.board.setupBoard("r3k2r/pppppppp/8888/PPPPPPPP/R3K2R w KkQq - 1 0");
	ai.board.generateMoveList<ALL>(moveList, white);
	assert(ai.board.castlingRights == 0b1111);
	auto rookMove = getMove(moveList, 0, 1);
	assert(rookMove != moveList.end());
	ai.board.makeMove<FULL>(*rookMove, white);
	ai.board.updateAllAttacks();
	ai.board.print();
	printBits(ai.board.castlingRights);
	assert(ai.board.castlingRights == 0b1011);

	moveList.clear();
	ai.board.generateMoveList<ALL>(moveList, white);
	assert(ai.board.castlingRights == 0b1011);
	rookMove = getMove(moveList, 1, 0);
	assert(rookMove != moveList.end());
	ai.board.makeMove<FULL>(*rookMove, white);
	ai.board.updateAllAttacks();
	ai.board.print();
	printBits(ai.board.castlingRights);
	assert(ai.board.castlingRights == 0b1011);
}

void UnitTest::testEnpassent()
{
	// Test enpassant move system
	AI ai("4k3/pppppppp8888PPPPPPPP/4K3 w - - 1 0", white, 10);
	MoveList movelist;
	ai.board.updateAllAttacks();

	auto hashKey = ai.board.hashKey;

	// Test all possible enpassant moves for black
	Move pawn2;            // pawn that is captured
	Move otherpawn;        // pawn, that performs enpassant
	MoveList::iterator ep; // enpassant
	for (int pos = 1; pos < 8; ++pos) {
		otherpawn = Move(h7 + pos, h4 + pos, MOVE, bp);
		pawn2 = Move(7 + pos, 23 + pos, PAWN2, wp);

		ai.board.makeMove<FULL>(otherpawn, black);
		ai.board.makeMove<FULL>(pawn2, white);
		ai.board.updateAllAttacks();

		movelist.clear();
		ai.board.generateMoveList<ALL>(movelist, black);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSANT; });
		assert(ep != movelist.end());
		assert(ep->from == h4 + pos && ep->to == 15 + pos && ep->pieces == bp);
		ai.board.makeMove<FULL>(*ep, black);
		ai.board.unMakeMove<FULL>(*ep, black);

		ai.board.unMakeMove<FULL>(pawn2, white);
		ai.board.unMakeMove<FULL>(otherpawn, black);
		ai.board.updateAllAttacks();

		assert(hashKey == ai.board.hashKey);
	}
	for (int pos = 0; pos < 7; ++pos) {
		otherpawn = Move(h7 + pos, h4 + pos, MOVE, bp);
		pawn2 = Move(9 + pos, 25 + pos, PAWN2, wp);

		ai.board.makeMove<FULL>(otherpawn, black);
		ai.board.makeMove<FULL>(pawn2, white);
		ai.board.updateAllAttacks();

		movelist.clear();
		ai.board.generateMoveList<ALL>(movelist, black);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSANT; });
		assert(ep != movelist.end());
		assert(ep->from == h4 + pos && ep->to == 17+pos && ep->pieces == bp);

		ai.board.makeMove<FULL>(*ep, black);
		ai.board.unMakeMove<FULL>(*ep, black);

		ai.board.unMakeMove<FULL>(pawn2, white);
		ai.board.unMakeMove<FULL>(otherpawn, black);
		ai.board.updateAllAttacks();

		assert(hashKey == ai.board.hashKey);
	}
	ai.board.updateAllAttacks();
	for (int pos = 0; pos < 7; ++pos) {
		otherpawn = Move(h2 + pos, h5 + pos, MOVE, wp);
		pawn2 = Move(g7 + pos, g5 + pos, PAWN2, bp);

		ai.board.makeMove<FULL>(otherpawn, white);
		ai.board.makeMove<FULL>(pawn2, black);
		ai.board.updateAllAttacks();

		movelist.clear();
		ai.board.generateMoveList<ALL>(movelist, white);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSANT; });
		assert(ep != movelist.end());
		assert(ep->from == h5 + pos && ep->to == g6 + pos && ep->pieces == wp);
		ai.board.makeMove<FULL>(*ep,   white);
		ai.board.unMakeMove<FULL>(*ep, white);

		ai.board.unMakeMove<FULL>(pawn2, black);
		ai.board.unMakeMove<FULL>(otherpawn, white);
		ai.board.updateAllAttacks();

		assert(hashKey == ai.board.hashKey);
	}
	for (int pos = 1; pos < 8; ++pos) {
		otherpawn = Move(h2 + pos, h5 + pos, MOVE, wp);
		pawn2 = Move(h7 - 1 + pos, h5 - 1 + pos, PAWN2, bp);

		ai.board.makeMove<FULL>(otherpawn, white);
		ai.board.makeMove<FULL>(pawn2, black);
		ai.board.updateAllAttacks();

		movelist.clear();
		ai.board.generateMoveList<ALL>(movelist, white);
		ep = find_if(movelist.begin(), movelist.end(), [](const Move& m) {return m.flags == ENPASSANT; });
		assert(ep != movelist.end());
		assert(ep->from == h5 + pos && ep->to == a5 + pos && ep->pieces == wp);
		ai.board.makeMove<FULL>(*ep,   white);
		ai.board.unMakeMove<FULL>(*ep, white);

		ai.board.unMakeMove<FULL>(pawn2, black);
		ai.board.unMakeMove<FULL>(otherpawn, white);
		ai.board.updateAllAttacks();

		assert(hashKey == ai.board.hashKey);
	}
	ai.board.print();
	Move m1(b7, b5, PAWN2, bp);
	Move m2(d7, d5, PAWN2, bp);
	Move dummy(h2, h3, MOVE, wp);

	pawn2 = Move(c2, c5, MOVE, wp);
	ai.board.makeMove<FULL>(pawn2, white);
	ai.board.print();
	cout << (int)ai.board.w_enpassant << endl;
	ai.board.makeMove<FULL>(m1, black);
	ai.board.print();
	cout << (int)ai.board.w_enpassant << endl;
	ai.board.makeMove<FULL>(dummy, white);
	ai.board.print();
	cout << (int)ai.board.w_enpassant << endl;
	ai.board.makeMove<FULL>(m2, black);
	ai.board.print();
	cout << (int)ai.board.w_enpassant << endl;
	ai.board.unMakeMove<FULL>(m2, black);
	ai.board.print();
	cout << (int)ai.board.w_enpassant << endl;
}

void UnitTest::testProm()
{
	AI ai("5n2/1P4P1/8/8/8/8/1p4p1/5N w - - 1 0", black, 10);
	ai.board.updateAllAttacks();
	ai.board.print();
	MoveList whiteMoves, blackMoves;
	ai.board.generateMoveList<ALL>(whiteMoves, white);
	ai.board.generateMoveList<ALL>(blackMoves, black);
	assert(count_if(whiteMoves.begin(), whiteMoves.end(), [](Move& move) {return move.flags == PROMOTION; })   == 8);
	assert(count_if(whiteMoves.begin(), whiteMoves.end(), [](Move& move) {return move.flags == C_PROMOTION; }) == 4);
	assert(count_if(blackMoves.begin(), blackMoves.end(), [](Move& move) {return move.flags == PROMOTION; })   == 8);
	assert(count_if(blackMoves.begin(), blackMoves.end(), [](Move& move) {return move.flags == C_PROMOTION; }) == 4);
	auto hashKey = ai.board.hashKey;
	for (auto& m : blackMoves) {
		ai.board.makeMove<FULL>(m, black);
		ai.board.unMakeMove<FULL>(m, black);
	}
	assert(hashKey == ai.board.hashKey);
	for (auto& m : whiteMoves) {
		ai.board.makeMove<FULL>(m, white);
		ai.board.unMakeMove<FULL>(m, white);
	}
	assert(hashKey == ai.board.hashKey);
}

void UnitTest::specialTest()
{
	AI ai("k7/8/3K4/8/8/6q1/8/5N2 w - - 1 0", white);

	ai.board.updateAllAttacks();
	ai.board.print();
	MoveList movelist;

	ai.board.generateMoveList<ALL>(movelist, white);

	exit(0);
}

void UnitTest::testHashing()
{
	/*
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
	*/
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

Benchmark::Benchmark() : totalTotalPerftMoveCount(0)
{
	genChessData data;
	data.genMoveData(); // Generates bitboards needed for move generation

	testBoard = Board("r4r1k/1bpq1p1n/p1np4/1p1Bb1BQ/P7/6R1/1P3PPP/1N2R1K1 w - - 1 0");

	// Now: 7r/8/8/2K5/R4k2/8/8/r6R b - - 1 0
	//testBoard.print();

	testBoard.updateAllAttacks();
	//MoveList moves;
	//testBoard.generateMoveList(moves, white, true);

	perftNodeCount = perftEPCount = perftMoveCount = perftCheckmateCount = totalPerftMoveCount = 0;
}

void Benchmark::testPerft(int maxdepth)
{
	if (maxdepth == -1) {
		// Check perft numbers
		auto hashKey = testBoard.hashKey;
		for (int d = 1; d < 10; d++) {
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
	testBoard.generateMoveList<ALL>(movelist, side);
	// Store pairs of root moves and number of possible moves after them
	bool checkmate = true;
	bool checkOnThisDepth = testBoard.wasInCheck;
	U64 pinnedOnThisDepth = testBoard.pinned;
	for (auto& move : movelist) {
		//try {
			//U64 testHash = testBoard.hashKey;
			testBoard.makeMove<FULL>(move, side);

			if (depth == 1) perftMoveCount++;
			if (testBoard.isKingLeftInCheck(side, move, checkOnThisDepth, pinnedOnThisDepth)) {
				testBoard.unMakeMove<FULL>(move, side);
				//if (testBoard.hashKey != testHash) throw string("Hash Error!");
				if (depth == 1) perftMoveCount--;
				continue;
			}

			if (move.flags == ENPASSANT && depth == 1) perftEPCount++;

			checkmate = false; // Moves were found
			perft(depth - 1, targetDepth, static_cast<color>(!side));
			testBoard.unMakeMove<FULL>(move, static_cast<color>(side));
			//if (testBoard.hashKey != testHash) throw string("Hash Error!");
			if (depth == targetDepth) {
				//cout << shortNotation(move) << ": " << perftMoveCount << endl;
				totalPerftMoveCount += perftMoveCount;
				totalTotalPerftMoveCount += perftMoveCount;
				perftMoveCount = 0;
			}
		//}
		//catch (const string& err) {
		//	cerr << err << endl;
		//	cerr << "Move " << moveString(move) << endl;
		//}
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
// commit 10e7c92dcbe4fdede499cd761ab3e443c9f26e56
//        Perft computation time: ~16.2 s (depth 5)
// commit 29b92d4610af4d53463308ba2cf2cf89fbd95c62
//        Perft computation time: ~15.5 s (depth 5)
// most recent commit:
//        Perft computation time: ~13.5 s (depth 5)

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
	Timer timer;
	timer.start();
	for (auto& testNum : perftTestData) {
		int c = 1;
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

DataBaseTest::DataBaseTest() : transposition_hash(ZobristHash(static_cast<size_t>(1e7))),
targetDepth(1), evalcnt(0), negaMaxCnt(0), storedBoards(0), hashAccess(0), moveCnt() {}

void DataBaseTest::start_Bratko_Kopec_Test()
{
	// Load and tokenize EPD position data
	clog << "Loading Bratko-Kopec-Database...\n";
	vector<vector<string>> dataBasepositions(24);
	string read;
	ifstream bkt("TestDataBase/bratkoKopec.dat", ios::in);
	if (!bkt.is_open()) {
		cerr << "Error: TestDataBase/bratkoKopec.dat missing!\n";
		exit(1);
	}

	int count = 0;
	auto ignore = boost::escaped_list_separator<char>("\\", " ", "\"");
	while (getline(bkt, read)) {
		boost::tokenizer<boost::escaped_list_separator<char>> tokenize(read, ignore);
		for (auto token = tokenize.begin(); token != tokenize.end(); ++token) {
			dataBasepositions[count].push_back(*token);
		}
		count++;
	}
	bkt.close();

	// start testing

	string boardStr;
	Move bestmove;

	int breakCount = 0;
	for (auto& test : dataBasepositions) {
		boardStr = test[0] + " " + test[1] + " " + test[2] + " " + test[3] + " 1 0";
		testBoard.setupBoard(boardStr);
		testBoard.updateAllAttacks();
		transposition_hash.clear();
		bestmove = getBestTestMove(test[1] == "b" ? black : white);
		if (++breakCount > 0) break;
	}
}

Move DataBaseTest::getBestTestMove(color forPlayer)
{
	Move bestMove;
	Timer timer;
	static int count = 1;
	cout << "Board " << count++ << endl;
	testBoard.print();
	auto oldHash = testBoard.hashKey;
	for (targetDepth = 1; targetDepth < 10; targetDepth++) {
		timer.start();
		bestMove = distributeNegaMax(forPlayer);
		timer.stop();
		cout << "Time: " << timer.getTime()*1e-6 << endl;
		cout << string(80, '~') << endl;
		cout << "Depth " << targetDepth << " best move = " << bestMove << endl;
		cout << "Search Info: \n";
		printf("\t%d\tEvaluations"
			"\n\t%d\tNegaMax Calls"
			"\n\t%d\tHashed boards"
			"\n\t%d\tHash Accesses"
			"\n\t%d\tPlayed Moves\n",
			evalcnt, negaMaxCnt, storedBoards, hashAccess, moveCnt);
		evalcnt = negaMaxCnt = hashAccess = moveCnt = 0;
	}
	assert(oldHash == testBoard.hashKey);
	return bestMove;
}

Move DataBaseTest::distributeNegaMax(color forPlayer)
{
	typedef pair<Move, int> moveValue;
	static vector<moveValue> RootMoveList;
	MoveList moveList;
	testBoard.updateAllAttacks();

	bool checkOnThisDepth = testBoard.wasInCheck;
	U64 pinnedOnThisDepth = testBoard.pinned;

	if (targetDepth == 1) {
		testBoard.generateMoveList<ALL>(moveList, forPlayer);

		checkOnThisDepth = testBoard.wasInCheck;
		pinnedOnThisDepth = testBoard.pinned;

		for (auto move = moveList.begin(); move != moveList.end(); ) {
			testBoard.makeMove<FULL>(*move, forPlayer);
			moveCnt++;
			if (testBoard.isKingLeftInCheck(forPlayer, *move, checkOnThisDepth, pinnedOnThisDepth)) {
				testBoard.unMakeMove<FULL>(*move, forPlayer);
				move = moveList.erase(move);
				continue;
			}
			testBoard.unMakeMove<FULL>(*move, forPlayer);
			move++;
		}
		// Initialize
		RootMoveList.resize(moveList.size());
		for (int i = 0; i < moveList.size(); ++i)
			RootMoveList[i] = pair<Move, int>(moveList[i], -oo);
	}

	for (auto& move_value : RootMoveList) {

		testBoard.makeMove<FULL>(move_value.first, forPlayer);
		auto entry = transposition_hash.getEntry(testBoard.hashKey);
		if (entry.search_depth >= targetDepth) {
			move_value.second = entry.value;
		}
		else {
			move_value.second = -NegaMax(-oo, oo, targetDepth - 1, forPlayer, !forPlayer);
		}
		testBoard.unMakeMove<FULL>(move_value.first, forPlayer);
	}

	auto bestMove = max_element(RootMoveList.begin(), RootMoveList.end(),
		[](const moveValue move1, const moveValue move2) {
		return move1.second < move2.second;
	});

	return bestMove->first;
}

int DataBaseTest::NegaMax(int alpha, int beta, int depth, color aiColor, color side)
{
	int oldAlpha = alpha;
	negaMaxCnt++;
	hashAccess++;
	auto &entry = transposition_hash.getEntry(testBoard.hashKey);

	if (entry.search_depth >= targetDepth) {
		if (entry.flags & EXACT_VALUE)
			return entry.value;
		else if (entry.flags & LOWER_BOUND)
			alpha = max(alpha, entry.value);
		else if (entry.flags & UPPER_BOUND)
			beta = min(beta, entry.value);
		if (alpha >= beta)
			return entry.value;
	}

	if (depth == 0) {
		evalcnt++;
		hashAccess--;
		return testBoard.evaluate(side);
	}

	int bestValue = -oo;
	MoveList movelist;
	movelist.reserve(32);
	testBoard.updateAllAttacks();
	testBoard.generateMoveList<ALL>(movelist, side);
	//stable_sort(movelist.begin(), movelist.end(), [&, this](const Move& m1, const Move& m2) { // Sort moves
	//	testBoard.makeMove<HASH_ONLY>(m1, side);
	//	int value1 = transposition_hash.getValue(testBoard.hashKey);
	//	testBoard.unMakeMove<HASH_ONLY>(m1, side);
	//	testBoard.makeMove<HASH_ONLY>(m2, side);
	//	int value2 = transposition_hash.getValue(testBoard.hashKey);
	//	testBoard.unMakeMove<HASH_ONLY>(m2, side);
	//	return value1 <= value2;
	//});
	//auto bestMove = max_element(movelist.begin(), movelist.end(), [&, this](const Move& m1, const Move& m2) {
	//	testBoard.makeMove<HASH_ONLY>(m1, side);
	//	int value1 = transposition_hash.getValue(testBoard.hashKey);
	//	testBoard.unMakeMove<HASH_ONLY>(m1, side);
	//	testBoard.makeMove<HASH_ONLY>(m2, side);
	//	int value2 = transposition_hash.getValue(testBoard.hashKey);
	//	testBoard.unMakeMove<HASH_ONLY>(m2, side);
	//	return value1 < value2;
	//});
	//iter_swap(movelist.begin(), bestMove);

	bool checkOnThisDepth = testBoard.wasInCheck;
	U64 pinnedOnThisDepth = testBoard.pinned;
	// Move ordering here
	for (auto move = movelist.begin(); move != movelist.end(); ) {
		//testBoard.print();
		testBoard.makeMove<FULL>(*move, side);
		moveCnt++;
		if (testBoard.isKingLeftInCheck(side, *move, checkOnThisDepth, pinnedOnThisDepth)) {
			testBoard.unMakeMove<FULL>(*move, side);
			move = movelist.erase(move);
			//move++;
			continue;
		}
		int currentValue = -NegaMax(-beta, -alpha, depth - 1, aiColor, !side);
		testBoard.unMakeMove<FULL>(*move, side);
		move++;
		bestValue = max(bestValue, currentValue);
		alpha = max(alpha, currentValue);
		if (alpha >= beta) {
			break; // Alpha beta cutoff
		}
	}

	if (movelist.empty()) {
		// This player has no possible legal moves
		testBoard.updateAllAttacks();
		if (testBoard.isKingInCheck(side))
			return -oo;          // Checkmate
		else return -oo + 100000; // Stalemate, slightly better than being checkmated
	}

	// Store board in transposition table
	storedBoards++;
	entry.value = bestValue;
	if (bestValue < oldAlpha)
		entry.flags = UPPER_BOUND;
	else if (bestValue >= beta)
		entry.flags = LOWER_BOUND;
	else
		entry.flags = EXACT_VALUE;
	entry.search_depth = targetDepth - depth;

	return bestValue;
}

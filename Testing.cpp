#include "Testing.h"

ChessError::ChessError(string msg) : errMsg(msg){}

void ChessError::what() const
{
	cerr << errMsg << '\n';
}

IntrinError::IntrinError(string msg, string name) : fname(name), ChessError(msg) {}

void IntrinError::what() const
{
	printf("Error in %s intrinsic\n", fname);
	cerr << errMsg << '\n';
}

UnitTest::UnitTest() {}

void UnitTest::testDefines() const 
{
  byte b = PIECE_PAIR(0xA, 0x5);
  if (MOV_PIECE(b) != 0xA)
    cerr << "TARGET_PIECE() Failed\n";
  if (TARGET_PIECE(b) != 0x5)
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
	const u64 randomCheckNum = 0xc69a3d1858e52a13;
	const u64 rotatedR[65] = {
		0xc69a3d1858e52a13,
		0xe34d1e8c2c729509,
		0xf1a68f4616394a84,
		0x78d347a30b1ca542,
		0x3c69a3d1858e52a1,
		0x9e34d1e8c2c72950,
		0x4f1a68f4616394a8,
		0x278d347a30b1ca54,
		0x13c69a3d1858e52a,
		0x9e34d1e8c2c7295,
		0x84f1a68f4616394a,
		0x4278d347a30b1ca5,
		0xa13c69a3d1858e52,
		0x509e34d1e8c2c729,
		0xa84f1a68f4616394,
		0x54278d347a30b1ca,
		0x2a13c69a3d1858e5,
		0x9509e34d1e8c2c72,
		0x4a84f1a68f461639,
		0xa54278d347a30b1c,
		0x52a13c69a3d1858e,
		0x29509e34d1e8c2c7,
		0x94a84f1a68f46163,
		0xca54278d347a30b1,
		0xe52a13c69a3d1858,
		0x729509e34d1e8c2c,
		0x394a84f1a68f4616,
		0x1ca54278d347a30b,
		0x8e52a13c69a3d185,
		0xc729509e34d1e8c2,
		0x6394a84f1a68f461,
		0xb1ca54278d347a30,
		0x58e52a13c69a3d18,
		0x2c729509e34d1e8c,
		0x16394a84f1a68f46,
		0xb1ca54278d347a3,
		0x858e52a13c69a3d1,
		0xc2c729509e34d1e8,
		0x616394a84f1a68f4,
		0x30b1ca54278d347a,
		0x1858e52a13c69a3d,
		0x8c2c729509e34d1e,
		0x4616394a84f1a68f,
		0xa30b1ca54278d347,
		0xd1858e52a13c69a3,
		0xe8c2c729509e34d1,
		0xf4616394a84f1a68,
		0x7a30b1ca54278d34,
		0x3d1858e52a13c69a,
		0x1e8c2c729509e34d,
		0x8f4616394a84f1a6,
		0x47a30b1ca54278d3,
		0xa3d1858e52a13c69,
		0xd1e8c2c729509e34,
		0x68f4616394a84f1a,
		0x347a30b1ca54278d,
		0x9a3d1858e52a13c6,
		0x4d1e8c2c729509e3,
		0xa68f4616394a84f1,
		0xd347a30b1ca54278,
		0x69a3d1858e52a13c,
		0x34d1e8c2c729509e,
		0x1a68f4616394a84f,
		0x8d347a30b1ca5427,
		0xc69a3d1858e52a13
	};
	const int indices[] = {
		63, 62, 58, 57, 55,
		52, 51, 49, 45, 44,
		43, 42, 40, 36, 35,
		30, 28, 27, 23, 22,
		21, 18, 16, 13, 11,
		9, 4, 1, 0
	};

	unsigned long index;
	clog << "\t::: Testing intrinsics :::\npopcnt...\n";
	int i = 0;
	assert(POPCOUNT(static_cast<u64>(0x0)) == 0);
	for (u64 p = 0x1; i < 64; p |= p << 1, ++i) 
		assert(POPCOUNT(p) == i + 1);
	assert(POPCOUNT(0xFFULL << 56) == 8); // Check if popcnt is really 64 bit
	clog << "bitscan...\n";
	BITSCANR64(index, 0x1ull << 10); // counts from least significant bit
	assert(index == 10);
	BITSCANR64(index, 0x0ull); // 0 if 0 (!)
	assert(index == 0);
	BITSCANR64(index, 0x1ull); // 0 if 1 (!)
	assert(index == 0);
	BITSCANR64(index, 0x1ull << 63); // 63 at msb
	assert(index == 63);
	clog << "rotr...\n";
	
	u64 testNum = randomCheckNum;
	for (int i = 0; i < 64; ++i) {
		testNum = ROTR64(testNum, 1);
		//cout << hex << rotatedR[i+1] << ' ' << testNum << endl;
		assert(testNum == rotatedR[i+1]);
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 16; ++i) {
		testNum = ROTR64(testNum, 4);
		assert(testNum == rotatedR[(i + 1) * 4]);
		//cout << hex << testNum << ' ' << randomCheckNum << endl;
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 8; ++i) {
		testNum = ROTR64(testNum, 8);
		assert(testNum == rotatedR[(i + 1) * 8]);
		//cout << hex << testNum << ' ' << randomCheckNum << endl;
	}
	assert(testNum == randomCheckNum);
	testNum = ROTR64(testNum, 70);
	testNum = ROTR64(testNum, 58);
	assert(testNum == randomCheckNum);

	clog << "rotl...\n";

	for (int i = 0; i < 64; ++i) {
		testNum = ROTL64(testNum, 1);
		//cout << hex << rotatedR[i+1] << ' ' << testNum << endl;
		assert(testNum == rotatedR[64 - i - 1]);
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 16; ++i) {
		testNum = ROTL64(testNum, 4);
		//cout << hex << rotatedR[60 - (i * 4)] << ' ' << testNum << endl;
		assert(testNum == rotatedR[60 - (i * 4)]);
	}
	assert(testNum == randomCheckNum);
	for (int i = 0; i < 8; ++i) {
		testNum = ROTL64(testNum, 8);
		assert(testNum == rotatedR[56 - (i * 8)]);
		//cout << hex << testNum << ' ' << randomCheckNum << endl;
	}
	assert(testNum == randomCheckNum);
	testNum = ROTL64(testNum, 70);
	testNum = ROTL64(testNum, 58);
	assert(testNum == randomCheckNum);

	clog << "bitloop...\n";
	int j = 0;
	BITLOOP(index, testNum) {
		assert(index == indices[j++]);
		//printBitboard(testNum);
		//printBitboard((randomCheckNum & ~(ULLONG_MAX << index) | BIT_AT(index)));
		assert(testNum == (randomCheckNum & ~(ULLONG_MAX << index) | BIT_AT(index)));
		//printBitboard(testNum);
	}

	cout << "Intrinsics passed all tests!\n";
}

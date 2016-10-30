#include "Testing.h"

UnitTest::UnitTest() {}

void UnitTest::testDefines() const {
  byte b = PIECE_PAIR(0xA, 0x5);
  if (MOV_PIECE(b) != 0xA)
    cerr << "TARGET_PIECE() Failed\n";
  if (TARGET_PIECE(b) != 0x5)
    cerr << "MOV_PIECE() Failed()\n";
}

void UnitTest::testIntrinsics() const {
  // Test popcount
  u64 p = 0x1ull;
  for (int i = 0; i < 63; ++i) {
    p <<= 1;
    p |= 0x1;
    if (POPCOUNT(p) != i + 2) {
      cerr << "POPCOUNT intrinsic failed\n";
      exit(1);
    }
  }
  unsigned long index = 0;
  u64 num = ULLONG_MAX;
  unsigned long temp,pos;
  BITLOOP(pos, num){
    printBitboard(num);
    cout << pos << endl;
  }
  cout << __builtin_ffsll(0x0) << endl;
}

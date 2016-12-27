#ifndef DEFINES_H
#define DEFINES_H

#include <cassert>
#include <cstdint>
#include <climits>

typedef unsigned uint;
typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  byte;

constexpr int oo = INT_MAX;

u64  inline bit_at(uint x)   { return 0x1ull << x; }        // Sets nth bit in number counting from least significant bit
u64  inline bit_at_rev(uint x) { return 0x1ull << (63 - x); } // Sets nth bit in number counting from most significant bit
byte inline piece_pair(byte X, byte Y) { return X | (Y << 4); } // Pairs up 4-bit piece information

// Move formatting macros:
byte inline move_piece(byte DATA)    { return DATA & 0xF; } // Piece that moves
byte inline target_piece(byte DATA) { return DATA >> 4; }   // Targeted piece
byte inline move_metadata(byte TYPE, byte DATA) { return TYPE | (DATA << 4); }

#ifdef _WIN32
	#ifndef __MACHINEX64
		#pragma message ("X64 ARCHITECTURE RECOMMENDED!\n COMPILATION MAY FAIL")
	#endif
	// Compiler intrinsics compatible with WIN32
	int  inline popcount(u64 x) { return __popcnt64(x); } // Population count, implicit conversion to int
	byte inline bitScan_rev64(unsigned long& index, u64 mask) { return _BitScanReverse64(&index, mask); } // Reverse Bitscan
	u64  inline rotate_l64(u64 mask, int amount) { return _rotl64(mask, amount); } // Rotate left  (64Bit)
	u64  inline rotate_r64(u64 mask, int amount) { return _rotr64(mask, amount); } // Rotate right (64Bit)

	#define BITLOOP(pos, mask) for (unsigned long pos = bitScan_rev64(pos, (mask)); \
																	bitScan_rev64(pos, (mask)); \
																	(mask) ^= 0x1ull << pos)
#elif __linux__
	// Compiler intrinsics compatible with gcc

	u64 inline popcount(u64 x){ return __builtin_popcountll(x); }
	u64 inline bitScan_rev64(unsigned long& index, u64 mask){
		index = mask == 0x0ull ? 0 : 63 - __builtin_clzll(mask);
		 return index;
	}

	u64 inline rotate_l64(u64& x, uint n){
		return (x << n) | (x >> (64 - n));
	}

	u64 inline rotate_r64(u64& x, uint n){
		return (x >> n) | (x << (64 - n));
	}

	#define BITLOOP(pos, mask) for (unsigned long pos = bitScan_rev64(pos, (mask)); \
																	bitScan_rev64(pos, (mask)); \
																	(mask) ^= 0x1ull << pos)
#endif

// For increasing readability:

#define BLACKLOOP(x) for (int x = 0; x < 6;  ++x)
#define WHITELOOP(x) for (int x = 6; x < 12; ++x)

#define BLACKLOOP_IT(x) for (auto& x = pieces.begin(); x != pieces.end() - 5; x++)
#define WHITELOOP_IT(x) for (auto& x = pieces.begin() + 6; x != pieces.end(); x++)

#endif

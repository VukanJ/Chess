#ifndef DEFINES_H
#define DEFINES_H

#include <cassert>
#include <cstdint>
#include <climits>
#include <functional>
#include <vector>

typedef unsigned uint;
typedef unsigned long ulong;
typedef uint64_t U64;
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t  byte;

constexpr int oo = INT_MAX; // Alpha-Beta infinity

U64  constexpr bit_at(uint x)     { return 0x1ull << x; }        // Sets nth bit in number counting from least significant bit
U64  constexpr bit_at_rev(uint x) { return 0x1ull << (63 - x); } // Sets nth bit in number counting from most significant bit
byte constexpr piece_pair(byte X, byte Y) { return X | (Y << 4); } // Pairs up 4-bit piece information

// Move formatting macros:
byte constexpr move_metadata(byte TYPE, byte DATA) { return TYPE | (DATA << 4); }

#ifdef _WIN32
	#ifndef __MACHINEX64
		#pragma message ("X64 ARCHITECTURE RECOMMENDED!\n COMPILATION MAY FAIL")
	#endif
	// Compiler intrinsics compatible with WIN32
	int  inline popcount(U64 x) { return (int)__popcnt64(x); } // Population count, implicit conversion to int
	byte inline bitScan_rev64(ulong& index, U64 const mask) {
		if (!_BitScanReverse64(&index, mask)){
			index = 0;
		}
		return (byte)index;
	} // Reverse Bitscan
	byte inline bitScan_fwd64(ulong& index, U64 const mask) {
		if (!_BitScanForward64(&index, mask)) {
			index = 0;
		}
		return (byte)index;
	} // Reverse Bitscan
	byte inline msb(U64 x) { ulong index; index = !_BitScanReverse64(&index, x) ? -1 : index; return (byte)index; }
	U64  inline rotate_l64(U64 mask, int amount) { return _rotl64(mask, amount); } // Rotate left  (64Bit)
	U64  inline rotate_r64(U64 mask, int amount) { return _rotr64(mask, amount); } // Rotate right (64Bit)

	#define for_bits(__pos, __mask) for (ulong __pos = msb(__mask); __mask; (__mask) ^= bit_at(__pos), __pos = msb(__mask))

#elif __linux__
	// Compiler intrinsics compatible with gcc

	U64 inline popcount(U64 x){ return __builtin_popcountll(x); }
	U64 inline bitScan_rev64(ulong& index, U64 mask){
		index = mask == 0x0ull ? 0 : 63 - __builtin_clzll(mask);
		 return index;
	}

	byte inline msb(U64 x) { ulong index; bitScan_rev64(index, x); return index; }

	U64 inline rotate_l64(U64& x, uint n){
		return (x << n) | (x >> (64 - n));
	}

	U64 inline rotate_r64(U64& x, uint n){
		return (x >> n) | (x << (64 - n));
	}

	#define BITLOOP(__pos, mask) for(ulong __pos = bitScan_rev64(__pos, mask); \
																	mask; mask ^= bit_at(bitScan_rev64(__pos, mask)), __pos = bitScan_rev64(__pos, mask))

	#define for_bits(__pos, __mask) for (ulong __pos = msb(__mask); __mask; (__mask) ^= bit_at(__pos), __pos = msb(__mask))
#endif

// For increasing readability:

#define for_black(x) for (int x = 0; x < 6;  ++x)
#define for_white(x) for (int x = 6; x < 12; ++x)
#define for_color(x, color) for(int x = 6 * color; x < 6 + 6 * color; ++x)

#define BLACKLOOP_IT(x) for (auto& x = pieces.begin(); x != pieces.end() - 5; x++)
#define WHITELOOP_IT(x) for (auto& x = pieces.begin() + 6; x != pieces.end(); x++)

#endif

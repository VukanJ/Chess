#ifndef DEFINES_H
#define DEFINES_H

#include <cassert>

typedef unsigned uint;

#define BIT_AT(x) (0x1ull << x)                               // Sets nth bit in number counting from least significant bit
#define BIT_AT_R(x) (0x1ull << (63-x))                        // Sets nth bit in number counting from most significant bit
#define PIECE_PAIR(X,Y) (X | (Y << 4))                        // Pairs up 4-bit piece information

// Move formatting macros:
#define MOV_PIECE(DATA) (DATA & 0xF)                          // Piece that moves
#define TARGET_PIECE(DATA) ((DATA & 0xF0) >> 4)               // Target piece
#define MOVE_TYPE(DATA) ((DATA & (0xFFull << 8))>>8)          // Move type
#define MOVE_TO(DATA) ((DATA & (0xFFull << 16))>>16)          // To-Square
#define MOVE_FROM(DATA) ((DATA & (0xFFull << 24))>>24)        // From-Square

#ifdef _WIN32
#ifndef __MACHINEX64
#pragma message ("X64 ARCHITECTURE RECOMMENDED!\n COMPILATION MAY FAIL")
#endif
	typedef unsigned __int64 u64;
	typedef unsigned __int32 u32;
	typedef unsigned __int16 u16;
	typedef unsigned __int8  byte;
	#define POPCOUNT(x) __popcnt64(x)                             // Population count
	#define BITSCANR64(index,mask) _BitScanReverse64(&index,mask) // Reverse Bitscan
	#define ROTL64(mask, amount) _rotl64(mask,amount)             // Rotate left (64Bit)
	#define ROTR64(mask, amount) _rotr64(mask,amount)             // Rotate right (64Bit)
#elif __linux__
#include <cstdint>
	typedef uint64_t u64;
	typedef uint32_t u32;
	typedef uint16_t u16;
	typedef uint8_t  byte;

	auto rotl64 = [&](u64& mask, uint amount) -> u64 {return ((mask << amount) | (mask >> (-amount & 31)));};
	auto rotr64 = [&](u64& mask, uint amount) -> u64 {return ((mask >> amount) | (mask << (-amount & 31)));};

	#define POPCOUNT(x) __builtin_popcount(x)
	#define BITSCANR64(index,mask) index = __builtin_clz(mask)

	#define ROTL64(mask, amount) rotl64(mask, amount);
	#define ROTR64(mask, amount) rotr64(mask, amount);
#endif

#define BITLOOP(pos,mask) for (unsigned long pos = BITSCANR64(pos, (mask)); BITSCANR64(pos, (mask)) ; (mask) ^= 1ull << pos)
#define BLACKLOOP(x) for (int x = 0; x < 6;  ++x)
#define WHITELOOP(x) for (int x = 6; x < 12; ++x)

#endif

#ifndef DATA_H
#define DATA_H

#include "misc.h"

static const vector<u64> KNIGHT_ATTACKS = {
	0x20400, 0x50800, 0xa1100, 0x142200, 0x284400, 0x508800, 0xa01000, 0x402000,
	0x2040004, 0x5080008, 0xa110011, 0x14220022, 0x28440044, 0x50880088,
	0xa0100010, 0x40200020, 0x204000402, 0x508000805, 0xa1100110a, 0x1422002214,
	0x2844004428, 0x5088008850, 0xa0100010a0, 0x4020002040, 0x20400040200,
	0x50800080500, 0xa1100110a00, 0x142200221400, 0x284400442800, 0x508800885000,
	0xa0100010a000, 0x402000204000, 0x2040004020000, 0x5080008050000,
	0xa1100110a0000, 0x14220022140000, 0x28440044280000, 0x50880088500000,
	0xa0100010a00000, 0x40200020400000, 0x204000402000000, 0x508000805000000,
	0xa1100110a000000, 0x1422002214000000, 0x2844004428000000, 0x5088008850000000,
	0xa0100010a0000000, 0x4020002040000000, 0x400040200000000, 0x800080500000000,
	0x1100110a00000000, 0x2200221400000000, 0x4400442800000000, 0x8800885000000000,
	0x100010a000000000, 0x2000204000000000, 0x4020000000000, 0x8050000000000,
	0x110a0000000000, 0x22140000000000, 0x44280000000000, 0x88500000000000,
	0x10a00000000000, 0x20400000000000
};

static const vector<u64> BISHOP_ATTACKS = {
	0x8040201008040200, 0x80402010080500, 0x804020110a00, 0x8041221400,
	0x182442800, 0x10204885000, 0x102040810a000, 0x102040810204000,
	0x4020100804020002, 0x8040201008050005, 0x804020110a000a, 0x804122140014,
	0x18244280028, 0x1020488500050, 0x102040810a000a0, 0x204081020400040,
	0x2010080402000204, 0x4020100805000508, 0x804020110a000a11, 0x80412214001422,
	0x1824428002844, 0x102048850005088, 0x2040810a000a010, 0x408102040004020,
	0x1008040200020408, 0x2010080500050810, 0x4020110a000a1120, 0x8041221400142241,
	0x182442800284482, 0x204885000508804, 0x40810a000a01008, 0x810204000402010,
	0x804020002040810, 0x1008050005081020, 0x20110a000a112040, 0x4122140014224180,
	0x8244280028448201, 0x488500050880402, 0x810a000a0100804, 0x1020400040201008,
	0x402000204081020, 0x805000508102040, 0x110a000a11204080, 0x2214001422418000,
	0x4428002844820100, 0x8850005088040201, 0x10a000a010080402, 0x2040004020100804,
	0x200020408102040, 0x500050810204080, 0xa000a1120408000, 0x1400142241800000,
	0x2800284482010000, 0x5000508804020100, 0xa000a01008040201, 0x4000402010080402,
	0x2040810204080, 0x5081020408000, 0xa112040800000, 0x14224180000000,
	0x28448201000000, 0x50880402010000, 0xa0100804020100, 0x40201008040201
};

static const vector<u64> QUEEN_ATTACKS = {
	0x81412111090503fe, 0x2824222120a07fd, 0x404844424150efb, 0x8080888492a1cf7,
	0x10101011925438ef, 0x2020212224a870df, 0x404142444850e0bf, 0x8182848890a0c07f,
	0x412111090503fe03, 0x824222120a07fd07, 0x4844424150efb0e, 0x80888492a1cf71c,
	0x101011925438ef38, 0x20212224a870df70, 0x4142444850e0bfe0, 0x82848890a0c07fc0,
	0x2111090503fe0305, 0x4222120a07fd070a, 0x844424150efb0e15, 0x888492a1cf71c2a,
	0x1011925438ef3854, 0x212224a870df70a8, 0x42444850e0bfe050, 0x848890a0c07fc0a0,
	0x11090503fe030509, 0x22120a07fd070a12, 0x4424150efb0e1524, 0x88492a1cf71c2a49,
	0x11925438ef385492, 0x2224a870df70a824, 0x444850e0bfe05048, 0x8890a0c07fc0a090,
	0x90503fe03050911, 0x120a07fd070a1222, 0x24150efb0e152444, 0x492a1cf71c2a4988,
	0x925438ef38549211, 0x24a870df70a82422, 0x4850e0bfe0504844, 0x90a0c07fc0a09088,
	0x503fe0305091121, 0xa07fd070a122242, 0x150efb0e15244484, 0x2a1cf71c2a498808,
	0x5438ef3854921110, 0xa870df70a8242221, 0x50e0bfe050484442, 0xa0c07fc0a0908884,
	0x3fe030509112141, 0x7fd070a12224282, 0xefb0e1524448404, 0x1cf71c2a49880808,
	0x38ef385492111010, 0x70df70a824222120, 0xe0bfe05048444241, 0xc07fc0a090888482,
	0xfe03050911214181, 0xfd070a1222428202, 0xfb0e152444840404, 0xf71c2a4988080808,
	0xef38549211101010, 0xdf70a82422212020, 0xbfe0504844424140, 0x7fc0a09088848281
};

static const vector<u64> KING_ATTACKS = {
	0x302, 0x705, 0xe0a, 0x1c14, 0x3828, 0x7050, 0xe0a0, 0xc040, 0x30203, 0x70507,
	0xe0a0e, 0x1c141c, 0x382838, 0x705070, 0xe0a0e0, 0xc040c0, 0x3020300,
	0x7050700, 0xe0a0e00, 0x1c141c00, 0x38283800, 0x70507000, 0xe0a0e000,
	0xc040c000, 0x302030000, 0x705070000, 0xe0a0e0000, 0x1c141c0000, 0x3828380000,
	0x7050700000, 0xe0a0e00000, 0xc040c00000, 0x30203000000, 0x70507000000,
	0xe0a0e000000, 0x1c141c000000, 0x382838000000, 0x705070000000, 0xe0a0e0000000,
	0xc040c0000000, 0x3020300000000, 0x7050700000000, 0xe0a0e00000000,
	0x1c141c00000000, 0x38283800000000, 0x70507000000000, 0xe0a0e000000000,
	0xc040c000000000, 0x302030000000000, 0x705070000000000, 0xe0a0e0000000000,
	0x1c141c0000000000, 0x3828380000000000, 0x7050700000000000, 0xe0a0e00000000000,
	0xc040c00000000000, 0x203000000000000, 0x507000000000000, 0xa0e000000000000,
	0x141c000000000000, 0x2838000000000000, 0x5070000000000000, 0xa0e0000000000000,
	0x40c0000000000000
};

static const vector<u64> noWrap = {
	0xffffffffffffff00, // down
	0x7f7f7f7f7f7f7f7f, // left
	0x00ffffffffffffff, // up
	0xfefefefefefefefe, // right

	0x7f7f7f7f7f7f7f00, // down left
	0x007f7f7f7f7f7f7f, // up left
	0x00fefefefefefefe, // right up
	0xfefefefefefefe00  // right down
};

static const int shift[8] = {8, -1, -8, 1, 7, -9, -7, 9};

#endif

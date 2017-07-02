#include "ZobristHash.h"

ZobristHash::ZobristHash()
{
	entries.resize(static_cast<size_t>(1e6));
	hashSize = static_cast<size_t>(1e6);
}

ZobristHash::ZobristHash(size_t _hashSize)
{
	entries.resize(_hashSize);
	hashSize = _hashSize;
}

ZobristHash::entry& ZobristHash::getEntry(const U64& key)
{
	//cout << hex << key % hashSize << dec << endl;
	return entries[key % hashSize];
}

int ZobristHash::getValue(const U64& key) const
{
	return entries[key % hashSize].value;
}

void ZobristHash::clear()
{
	for (auto& entry : entries) {
		// Invalidate entry:
		entry.search_depth = -1;
	}
}

// struct entry

ZobristHash::entry::entry() : value(-oo), search_depth(-1), flags(0x0) {}
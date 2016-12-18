#include "ZobristHash.h"

Zob_Hash::Zob_Hash()
{
	entries.reserve(static_cast<u64>(1e6));
	entries.resize(1e6);
}

Zob_Hash::Zob_Hash(size_t hashSize)
{
	entries.reserve(static_cast<u64>(hashSize));
	entries.resize(hashSize);
}

void Zob_Hash::addEntry(const u64 key, int value, int depth)
{
	int index = static_cast<int>(key % (u64)entries.size());
	if (entries[index].search_depth < depth){
		entries[index].search_depth = depth;
		entries[index].value = value;
	}
}

bool Zob_Hash::hasEntry(const u64 key) const
{
	auto index = (unsigned)(key % (u64)entries.size());
	return (entries[index].search_depth != -1) ? true : false;
}

bool Zob_Hash::getEntry(const u64 key, int& value) const
{
	auto index = (unsigned)(key % (u64)entries.size());
	if (entries[index].search_depth != -1){
		value = entries[index].value;
		return true;
	}
	return false;
}

bool Zob_Hash::getEntry(const u64 key, int& value, int& depth) const
{
	int index = (unsigned)(key % (u64)entries.size());
	if (entries[index].search_depth != -1){
		value = entries[index].value;
		depth = entries[index].search_depth;
		return true;
	}
	return false;
}

vector<vector<u64>> Zob_Hash::getRandomSet()
{
	random_device r_device;
	mt19937_64 generator(r_device());
	uniform_int_distribution<u64> distr;
	vector<vector<u64>> randSet = vector<vector<u64>>(14, vector<u64>(64, 0));
	// Index 0-11: Piece type
	for (auto& r1 : randSet)
		for (auto& r2 : r1)
			r2 = distr(generator);
	return randSet;
}
#include "ZobristHash.h"

Zob_Hash::Zob_Hash()
{
	entries.reserve(static_cast<u64>(1e2));
}

void Zob_Hash::addEntry(const u64 key, float value, int depth)
{
	int index = static_cast<int>(key % (u64)entries.size());
	if (entries[index].search_depth < depth){
		entries[index].search_depth = depth;
		entries[index].value = value;
	}
}

bool Zob_Hash::hasEntry(const u64 key) const
{
	int index = key % (u64)entries.size();
	return (entries[index].search_depth != -1) ? true : false;
}

bool Zob_Hash::getEntry(const u64 key, float& value) const
{
	int index = key % (u64)entries.size();
	if (entries[index].search_depth != -1){
		value = entries[index].value;
		return true;
	}
	return false;
}

bool Zob_Hash::getEntry(const u64 key, float& value, int& depth) const
{
	int index = key % (u64)entries.size();
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
	for (auto& r1 : randSet)
		for (auto& r2 : r1)
			r2 = distr(generator);
	// make last vector shorter
	return randSet;
}
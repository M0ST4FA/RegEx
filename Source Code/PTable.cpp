#include <assert.h>

#include "PTable.h"

std::string m0st4fa::stringfy(const LRTableEntryType type)
{
	assert((size_t)LRTableEntryType::TET_COUNT == 5);

	static const char* names[] = {
		"SHIFT",
		"REDUCE",
		"GOTO",
		"ACCEPT",
		"ERROR"
	};

	if (type == LRTableEntryType::TET_COUNT)
		return std::format("Number of types is {}", (size_t)LRTableEntryType::TET_COUNT);

	return std::string(names[(size_t)type]);
}
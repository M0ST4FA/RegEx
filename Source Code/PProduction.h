#pragma once
#include <string>
#include <vector>

#include "PStack.h"

namespace m0st4fa {

	// Production
	template <typename SymbolT>
	struct ProductionRecord {
		// TODO: demand that the prodHead be a non-terminal
		SymbolT prodHead = SymbolT{};
		std::vector<StackElement<SymbolT>> prodBody;

		ProductionRecord& operator=(const ProductionRecord& other) {
			prodHead = other.prodHead;
			prodBody = other.prodBody;
			return *this;
		}

	};

	
}

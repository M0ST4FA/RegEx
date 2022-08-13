#pragma once

#include "PEnum.h"


// defined constants
#define EXTRACT_TERMINAL(stackElement) (size_t)stackElement.as.gramSymbol.as.terminal
#define EXTRACT_VARIABLE(stackElement) (size_t)stackElement.as.gramSymbol.as.nonTerminal

namespace m0st4fa {

	template <typename SymbolT>
	struct StackElement {
		StackElementType type;

		union {
			SymbolT gramSymbol;
		} as;

		StackElement& operator=(const StackElement& other) {
			type = other.type;
			switch (type) {
			case SE_GRAM_SYMBOL:
				as.gramSymbol = other.as.gramSymbol;
				break;
			}
			return *this;
		}

	};

	
}
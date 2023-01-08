#include "regexTypedefs.h"

namespace m0st4fa::regex {
	SymbolType toSymbol(const Terminal terminal)
	{
		return SymbolType{ .isTerminal = true, .as {.terminal = terminal} };
	}

	SymbolType toSymbol(const Variable variable)
	{
		return SymbolType{ .isTerminal = false, .as {.nonTerminal = variable} };
	}
}
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

	template <typename SymbolT>
	std::ostream& operator<<(std::ostream& os, const ProductionRecord<SymbolT>& prod) {

		std::cout << prod.prodHead << " -> ";

		// body
		for (const StackElement<SymbolT>& symbol : prod.prodBody)
			std::cout << symbol.as.gramSymbol << " ";
	

		return std::cout << "\n";

	};

	// Symbol
	template <typename TerminalT, typename VariableT>
	struct GrammaticalSymbol {
		bool isTerminal = false;

		union {
			TerminalT terminal;
			VariableT nonTerminal;
		} as;
		
		template <typename TokenT>
			requires requires (TokenT tok) { tok.name; }
		bool operator==(const TokenT& token) const {

			if (isTerminal)
				return as.terminal == token.name;

			return false;
		}

		bool operator==(const GrammaticalSymbol& other) {

			if (isTerminal)
				return as.terminal == other.as.terminal;
			else
				return as.nonTerminal == other.as.nonTerminal;
			
		};

	};

	template <typename TerminalT, typename VariableT>
	using Symbol = GrammaticalSymbol<TerminalT, VariableT>;

	template <typename TerminalT, typename VariableT>
	std::ostream& operator<<(std::ostream& os, const Symbol<TerminalT, VariableT>& symbol) {

		if (symbol.isTerminal)
			std::cout << symbol.as.terminal;
		else
			std::cout << symbol.as.nonTerminal;

		return os;
		
	}
	
}

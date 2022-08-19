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

		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {

			std::string str = this->prodHead.toString() + " -> ";

			// body
			for (const StackElement<SymbolT>& symbol : this->prodBody)
				str+= symbol.as.gramSymbol.toString() + " ";

			return str;
		}

	};

	template <typename SymbolT>
	std::ostream& operator<<(std::ostream& os, const ProductionRecord<SymbolT>& prod) {

		return std::cout << prod.toString() << "\n";
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

			if (this->isTerminal)
				return this->as.terminal == token.name;

			return false;
		}

		bool operator==(const GrammaticalSymbol& other) {

			if (isTerminal)
				return as.terminal == other.as.terminal;
			else
				return as.nonTerminal == other.as.nonTerminal;
			
		};
		
		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {

			return this->isTerminal ? stringfy(this->as.terminal) : stringfy(this->as.nonTerminal);

		}

	};

	template <typename TerminalT, typename VariableT>
	using Symbol = GrammaticalSymbol<TerminalT, VariableT>;

	template <typename TerminalT, typename VariableT>
	std::ostream& operator<<(std::ostream& os, const Symbol<TerminalT, VariableT>& symbol) {

		return os << symbol.toString();
		
	}
	
}

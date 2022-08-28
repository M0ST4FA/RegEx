#pragma once
#include <string>
#include <vector>

#include "PStack.h"

namespace m0st4fa {

	// Production
	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	struct ProductionRecord {

		using StackElement = StackElement<SymbolT, SynthesizedT, ActionT>;
		using Stack = Stack<SymbolT, SynthesizedT, ActionT>;

		// TODO: demand that the prodHead be a non-terminal
		SymbolT prodHead = SymbolT{};
		std::vector<StackElement> prodBody;

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
			for (const StackElement& symbol : this->prodBody) {

				switch (symbol.type) {
				case SET_GRAM_SYMBOL:
					str += symbol.as.gramSymbol.toString() + " ";
					break;

				case SET_SYNTH_RECORD:
					str += symbol.as.synRecord.toString() + " ";
					break;

				case SET_ACTION_RECORD:
					str += symbol.as.actRecord.toString() + " ";
					break;

				}
			}


			return str;
		}

	};

	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	std::ostream& operator<<(std::ostream& os, const ProductionRecord<SymbolT, SynthesizedT, ActionT>& prod) {

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


	// ALIASES
	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	using ProdVec = std::vector<ProductionRecord<SymbolT, SynthesizedT, ActionT>>;

}

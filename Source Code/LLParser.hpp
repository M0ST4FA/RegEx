#pragma once
// std includes
#include <vector>


// local includes
#include "Parser.h"

namespace m0st4fa {

	template <typename SymbolT, typename TokenT,
		typename ParsingTableT = LLParsingTable<>, typename FSMTableT = FSMTable<>,
		typename InputT = std::string> 
	class LLParser : public Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT> {

		// a parsing table entry should contain indecies into this vector of productions
		std::vector<ProductionRecord<SymbolT>> m_ProdRecords;
		
	public:
		
		// constructors
		LLParser() = default;
		LLParser(
			const std::vector<ProductionRecord<SymbolT>>& prodRecords,
			const SymbolT& startSymbol,  
			const ParsingTableT& parsingTable, 
			const LexicalAnalyzer<TokenT, FSMTableT>& lexer
			):
			Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT> 
					{lexer, parsingTable, startSymbol}, 
			m_ProdRecords{ prodRecords }
		{};

		// methods
		/**
		* @output If w is in L(G), a leftmost derivation of w; otherwise, an error indication.
		* It might execute actions during the leftmost derivation, for example, to make a parsing or syntax tree.
		*/
		ParserResult parse(ExecutionOrder);


	};


	// IMPLEMENTATIONS
	template<typename SymbolT, typename TokenT, typename ParsingTableT, typename FSMTableT, typename InputT>
	ParserResult LLParser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>::parse(ExecutionOrder exeuctionOrder)
	{
		ParserResult res;
		std::vector<StackElement<SymbolT>> stack;

		// Initalize the stack, such that the parser is in the initial configuration
		// push back the start symbol
		stack.push_back({SE_GRAM_SYMBOL, {this->getStartSymbol()}});
		
		/**
		* Loop until the stack is empty.
		* Consider the current symbol on top of the stack and the current input.
		* Make the parsing descision based on these two tokens.
		* Whenever a symbol is matched, it is popped off the stack.
		* The purpose is to pop the start symbol off the stack (to match it, leaving the stack empty) and not produce any errors.
		*/
		TokenT currInputToken = this->getLexicalAnalyzer().getNextToken();
		while (-not stack.empty()) {

			// get the current symbol on top of the stack, pop it and get the next input token
			const StackElement top = stack.back();
			const SymbolT topSymbol = top.as.gramSymbol;
			stack.pop_back();


			// if the symbol at the top of the stack is a terminal symbol
			if (topSymbol.isTerminal) {
				// match it explicitly
				bool matched = (topSymbol == currInputToken);

				std::clog << "Matched " << topSymbol << " with " << currInputToken << ": " << std::boolalpha << matched << std::endl;
				currInputToken = this->getLexicalAnalyzer().getNextToken();

				if (-not matched); // TODO: error handling
				
			}
			// if the symbol is a non-terminal symbol
			else {

				// get the production record for the current symbol and input
				const TableEntry tableEntry = this->m_Table[EXTRACT_VARIABLE(top)][(size_t)currInputToken.name];

				// Caution: this is a reference
				const auto& prod = this->m_ProdRecords[tableEntry.prodIndex];
				const auto& prodBody = prod.prodBody;

				// if the production record is empty
				if (prodBody.empty()); // TODO: error

				// push the body of the production on top of the stack
				for (auto it = prodBody.rbegin(); it != prodBody.rend(); ++it) {
					StackElement<SymbolT> se = *it;
					stack.push_back(se);
				};
				std::cout << "Expanded " << topSymbol << " with " << currInputToken << ": " << prod << std::endl;
				
			}
			
		}

		return res;
	}


}

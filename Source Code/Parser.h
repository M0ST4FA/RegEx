#pragma once
#include <concepts>

#include "LexicalAnalyzer.h"
#include "PDataStructs.h"

namespace m0st4fa {
	// Parser class

	template <typename ParsingTableT, typename SymbolT, typename TokenT>
	concept ParserRequirments = requires (SymbolT gramSym, TokenT token) {
		////require the parsing table to be callable
		//ParsingTableT(gramSym, token);

		//// require SymbolT to have a 'isTerminal' property
		//gramSym.isTerminal;

		//// require SymbolT to be comparable by equality with TokenT and returns a boolean
		//{gramSym == token} noexcept -> std::same_as<bool>;

		//// require SymbolT to have operator== and operator<
		//gramSym == gramSym;
		//gramSym < gramSym;
		//

		true;
	};

	/**
	* @input Input buffer w (managed by the lexical analyzer) and parsing table M for grammer G.
	*/
	template <typename SymbolT, typename TokenT, 
		typename ParsingTableT, typename FSMTableT = FSMTable<50, 'z'>, // set them to have a default value
		typename InputT = std::string>
		requires ParserRequirments<ParsingTableT, SymbolT, TokenT>
	class Parser {
		LexicalAnalyzer<TokenT, FSMTableT> m_LexicalAnalyzer;
		SymbolT m_StartSymbol;

	protected:
		ParsingTableT m_Table;
		Logger m_Logger;

		LexicalAnalyzer<TokenT, FSMTableT>& getLexicalAnalyzer() { return m_LexicalAnalyzer; }
		SymbolT getStartSymbol() { return m_StartSymbol; }

	public:

		Parser() = default;
		Parser(
			const LexicalAnalyzer<TokenT, FSMTableT>& lexer, 
			const ParsingTableT& parsingTable, 
			const SymbolT& startSymbol) :
			m_LexicalAnalyzer{ lexer }, m_Table{ parsingTable }, m_StartSymbol{ startSymbol }
		{
			// TODO: check for argument correctness

		};


	};


};

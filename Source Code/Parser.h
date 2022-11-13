#pragma once
#include <concepts>

#include "LexicalAnalyzer.h"
#include "PDataStructs.h"

namespace m0st4fa {
	// Parser class

	// TODO: work on this concept
	template <typename ParsingTableT>
	concept ParserRequirments = requires (ParsingTableT table) {
		//require the parsing table to be callable
		//table[EXTRACT_VARIABLE(gramSym)][EXTRACT_TERMINAL(token)];

		// require SymbolT to be comparable by equality with TokenT and returns a boolean
		//{gramSym == token} -> std::same_as<bool>;
		// require SymbolT to have operator== and operator<
		//gramSym == gramSym;

		true;
	};

	/**
	* @input Input buffer w (managed by the lexical analyzer) and parsing table M for grammer G.
	*/
	template <typename LexicalAnalyzerT,
		typename SymbolT,
		typename ParsingTableT, typename FSMTableT = FSMTable<>, // set them to have a default value
		typename InputT = std::string>
		requires ParserRequirments<ParsingTableT>
	class Parser {
		using TokenType = decltype(LexicalAnalyzerT{}.getNextToken());

		mutable LexicalAnalyzerT m_LexicalAnalyzer;
		SymbolT m_StartSymbol;

	protected:
		mutable ParsingTableT p_Table;
		Logger p_Logger;

		// set the maximum number of errors that you recover from
		static constexpr size_t ERR_RECOVERY_LIMIT = 1;

		LexicalAnalyzerT& getLexicalAnalyzer() { return m_LexicalAnalyzer; }
		TokenType getNextToken() { return this->m_LexicalAnalyzer.getNextToken(); }
		SymbolT getStartSymbol() const { return m_StartSymbol; }

	public:

		Parser() = default;
		Parser(
			const LexicalAnalyzerT& lexer, 
			const ParsingTableT& parsingTable, 
			const SymbolT& startSymbol) :
			m_LexicalAnalyzer{ lexer }, p_Table{ parsingTable }, m_StartSymbol{ startSymbol }
		{
			// TODO: check for argument correctness

		};


	};


};

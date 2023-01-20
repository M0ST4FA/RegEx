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

		LexicalAnalyzerT* mp_LexicalAnalyzer;
		SymbolT m_StartSymbol;

	protected:
		mutable ParsingTableT p_Table;
		Logger p_Logger;

		// set the maximum number of errors that you recover from
		static constexpr size_t ERR_RECOVERY_LIMIT = 5;

		std::string_view get_source_code() const { return this->mp_LexicalAnalyzer->getSourceCode(); }
		TokenType get_next_token() const { return this->mp_LexicalAnalyzer->getNextToken((unsigned)LA_FLAG::LAF_ALLOW_WHITE_SPACE_CHARS); }
		SymbolT get_start_symbol() const { return m_StartSymbol; }

	public:

		Parser() = default;
		Parser(
			LexicalAnalyzerT& lexer, 
			const ParsingTableT& parsingTable, 
			const SymbolT& startSymbol) :
			mp_LexicalAnalyzer{ &lexer }, p_Table{ parsingTable }, m_StartSymbol{ startSymbol }
		{
			// TODO: check for argument correctness

		};


	};


};

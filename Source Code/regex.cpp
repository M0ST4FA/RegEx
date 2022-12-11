#include "regex.h"

namespace m0st4fa {

	// INITIALIZATION
	namespace regex {
				
		ParsingTableType RegularExpression::_get_parsing_table()
		{
			ParserGeneratorType parserGen{ RegularExpression::_get_grammar(), toSymbol(Variable::V_REGEX) };

			return parserGen.generateCLRParser();
		}
		inline constexpr SymbolType RegularExpression::_get_start_symbol()
		{
			return SymbolType{ .isTerminal = false, .as {.nonTerminal = Variable::V_REGEX} };
		}
		constexpr TokenFactType RegularExpression::_get_token_factory()
		{
			return TokenFactType();
		}
		DFAType RegularExpression::_get_automaton()
		{
			return DFAType();
		}

		const ParsingTableType RegularExpression::PARSING_TABLE = RegularExpression::_get_parsing_table();
		const SymbolType RegularExpression::START_SYMBOL = RegularExpression::_get_start_symbol();
		const TokenFactType RegularExpression::TOKEN_FACTORY = RegularExpression::_get_token_factory();
		const DFAType RegularExpression::AUTOMATON = RegularExpression::_get_automaton();

	}

	// BEHAVIOR IMPLEMENTATION
	namespace regex {
		RegularExpressionResult RegularExpression::exec(const std::string& source)
		{
			RegularExpressionResult res;



			return res;
		}

		bool RegularExpression::match(const std::string& source)
		{
			return false;
		}

	}

}


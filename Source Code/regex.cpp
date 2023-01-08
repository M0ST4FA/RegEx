#include "regex.h"
#include "regexDFA.h"

namespace m0st4fa {
	// INITIALIZATION
	namespace regex {
		ParsingTableType RegularExpression::_get_parsing_table()
		{
			ParserGeneratorType parserGen{ RegularExpression::_get_grammar(), toSymbol(Variable::V_REGEX) };

			return parserGen.generateLALRParser(ConflictPolicy::CP_PREFER_SHIFT);
		}
		constexpr SymbolType RegularExpression::_get_start_symbol()
		{
			return SymbolType{ .isTerminal = false, .as {.nonTerminal = Variable::V_REGEX} };
		}
		constexpr TokenFactType RegularExpression::_get_token_factory()
		{
			return &_token_factory;
		}
		DFAType RegularExpression::_get_automaton()
		{
			state_set_t fstates;
			for (size_t i = 2; i < 20; i++)
				fstates.insert(i);

			return DFAType{ fstates, _get_dfa_table() };
		}

		const ParsingTableType RegularExpression::PARSING_TABLE = RegularExpression::_get_parsing_table();
		const SymbolType RegularExpression::START_SYMBOL = RegularExpression::_get_start_symbol();
		const TokenFactType RegularExpression::TOKEN_FACTORY = RegularExpression::_get_token_factory();
		const DFAType RegularExpression::AUTOMATON = RegularExpression::_get_automaton();
	}

	// BEHAVIOR IMPLEMENTATION
	namespace regex {
		RegularExpressionResult RegularExpression::exec(std::string_view source)
		{
			RegularExpressionResult res;

			size_t x = 0;
			std::cin >> x;

			return res;
		}

		bool RegularExpression::match(std::string_view source)
		{
			return false;
		}
	}
}
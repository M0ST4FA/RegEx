#include "regex.h"

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
			FSMStateSetType fstates;
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
		
		void RegularExpression::_parse_flags(std::string_view flags) {
			static_assert((unsigned)Flag::F_COUNT == 4);

			// in case there are now flags
			if (flags.empty()) {
				this->m_Flags = (unsigned)Flag::F_NONE;
				return;
			}

			// if there are any flags
			for (const char f : flags) {
				this->m_Flags &= ~(unsigned)Flag::F_NONE;
				_check_flag_conflict(f);

				switch (f) {
				case 'g': {
					this->m_Flags |= (unsigned)Flag::F_GLOBAL;
					break;
				}

				case 'y': {
					this->m_Flags |= (unsigned)Flag::F_STIKY;
					break;
				}

				case 'i': {
					this->m_Flags |= (unsigned)Flag::F_CASE_INSENSITIVE;
					break;
				}

				default:
					std::cout << "\nUnrecognized flag in _parse_flags\n";
					std::abort();
				}

			}

		};

		void RegularExpression::_check_flag_conflict(const char flag) const
		{
			static_assert((unsigned)Flag::F_COUNT == 4);

			bool invalid = false;

			bool global = this->m_Flags & (unsigned)Flag::F_GLOBAL;
			bool sticky = this->m_Flags & (unsigned)Flag::F_STIKY;
			bool caseInsensitive = this->m_Flags & (unsigned)Flag::F_CASE_INSENSITIVE;

			switch (flag) {
			case 'g':
			case 'y':
				if (global || sticky)
					invalid = true;
				break;
			case 'i':
				if (caseInsensitive)
					invalid = true;
				break;
			default:
				std::cout << "\nUnrecognized flag in _check_flag_conflict\n";
				std::abort();
			}

			if (!invalid)
				return;

			// if the flags are invalid
			this->p_Logger.log(LoggerInfo::ERR_INVALID_FLAGS, std::format("This sequence of flags `{}` is invalid. A flag cannot exist multiple times and certain flags conflict with each other.", this->m_strFlags));
			throw InvalidRegexFlags{};
		}

		RegularExpressionResult RegularExpression::exec(std::string_view source)
		{
			RegularExpressionResult res;

			return res;
		}

		bool RegularExpression::match(std::string_view source)
		{
			auto res = this->m_PatternAutomaton.simulate(source, this->m_FSMMode);

			return res.accepted;
		}
	}
}
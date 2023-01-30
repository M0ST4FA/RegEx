#pragma once
#include <string>

#include "LRParser.hpp"
#include "regexTypedefs.h"
#include "common.h"

namespace m0st4fa {
	namespace regex {
		typedef class RegularExpression regex;
		typedef struct RegularExpressionResult regexRes;

		class RegularExpression {
			// data members
			std::string m_Pattern;
			mutable DFAType m_PatternAutomaton;
			index_t m_Index = 0;
			std::string_view m_strFlags = "";
			size_t m_Flags = (size_t)Flag::F_NONE;
			FSM_MODE m_FSMMode = FSM_MODE::MM_LONGEST_PREFIX;
			mutable LexicalAnalyzerType* m_Lexer{ nullptr };
			mutable ParserType* m_Parser{ nullptr };

			static const ParsingTableType PARSING_TABLE;
			static const SymbolType START_SYMBOL;
			static const TokenFactType TOKEN_FACTORY;
			static const DFAType AUTOMATON;
			// private methods

			inline void _allocate_la_parser() {
				// Allocate the lexical analyzer and the parser
				void* storage = std::malloc(sizeof(LexicalAnalyzerType) + sizeof(ParserType));
				std::cout << "Allocating `" << sizeof(LexicalAnalyzerType) + sizeof(ParserType) / 1e3 << "` KiBs\n";

				this->m_Lexer = (LexicalAnalyzerType*)storage;
				this->m_Parser = (ParserType*)((size_t)storage + sizeof(LexicalAnalyzerType));
			}
			void _init_regex() {
				_allocate_la_parser();

				// Initialize the lexical analyzer and the parser
				this->m_Pattern += REGEX_END_MARKER;
				this->m_Lexer = new(this->m_Lexer) LexicalAnalyzerType{ AUTOMATON, TOKEN_FACTORY, this->m_Pattern };
				this->m_Parser = new(this->m_Parser) ParserType{ *this->m_Lexer, PARSING_TABLE, START_SYMBOL };

				// Initialize the pattern automaton that will be used for matching string inputs
				const auto FSMData = this->m_Parser->parse<std::pair<FSMStateSetType, DFATableType>>();

				const FSMStateSetType& finalStates = FSMData.first;
				const DFATableType& transTable = FSMData.second;
				unsigned FSMFlags = _get_fsm_flags();

				this->m_PatternAutomaton = DFAType{ finalStates, transTable, FSMFlags };
			}

			void _check_flag_conflict(const char) const;
			void _parse_flags(std::string_view flags);

			inline unsigned _get_fsm_flags() {
				return 0;
			};
			inline void _set_FSM_sim_mode() {

				if (this->m_Flags == (unsigned)Flag::F_NONE)
					this->m_FSMMode = FSM_MODE::MM_WHOLE_STRING;
				else if (this->m_Flags & (unsigned)Flag::F_GLOBAL)
					this->m_FSMMode = FSM_MODE::MM_LONGEST_SUBSTRING;
				else if (this->m_Flags & (unsigned)Flag::F_STIKY)
					this->m_FSMMode = FSM_MODE::MM_LONGEST_PREFIX;

			};

			static DFATableType _get_dfa_table();
			static TokenType _token_factory(state_t, InputType);
			static GrammarType _get_grammar();
			static ParsingTableType _get_parsing_table();
			static constexpr SymbolType _get_start_symbol();
			static constexpr TokenFactType _get_token_factory();
			static DFAType _get_automaton();

		protected:
			Logger p_Logger;

		public:

			RegularExpression() = default;
			RegularExpression(std::string_view pattern, [[maybe_unused]] std::string_view flags = "") : m_Pattern{ pattern }, m_strFlags{flags} {

				_parse_flags(flags);
				_set_FSM_sim_mode();
				_init_regex();

			};

			~RegularExpression() {
				std::free(this->m_Lexer);
			}

			// functional methods
			RegularExpressionResult exec(std::string_view);
			bool match(std::string_view);

			// getters
			std::string_view getPattern() const { return std::string_view(m_Pattern.c_str(), m_Pattern.size() - 1); };
			index_t getIndex() const { return m_Index; };
			std::string_view getFlags() const { return this->m_strFlags; };
		};

		struct RegularExpressionResult {
		};
	}
};
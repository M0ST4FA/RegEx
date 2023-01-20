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
			size_t m_Flags = (size_t)Flag::F_NONE;
			FSM_MODE m_FSMMode = FSM_MODE::MM_LONGEST_PREFIX;
			mutable LexicalAnalyzerType* m_Lexer{ nullptr };
			mutable ParserType* m_Parser{ nullptr };

			static const ParsingTableType PARSING_TABLE;
			static const SymbolType START_SYMBOL;
			static const TokenFactType TOKEN_FACTORY;
			static const DFAType AUTOMATON;
			// private methods

			std::string _flags_to_string() const {
				return "TODO: HANDLE FLAGS_TO_STRING";
			}

			static DFATableType _get_dfa_table();
			static TokenType _token_factory(state_t, InputType);
			static GrammarType _get_grammar();
			static ParsingTableType _get_parsing_table();
			static constexpr SymbolType _get_start_symbol();
			static constexpr TokenFactType _get_token_factory();
			static DFAType _get_automaton();

		public:

			RegularExpression() = default;
			RegularExpression(std::string_view pattern, [[maybe_unused]] std::string_view flags = "") : m_Pattern{ pattern } {

				// Allocate the lexical analyzer and the parser
				void* storage = std::malloc(sizeof(LexicalAnalyzerType) + sizeof(ParserType));
				std::cout << "Allocating `" << sizeof(LexicalAnalyzerType) + sizeof(ParserType) / 1e3 << "` KiBs\n";

				this->m_Lexer = (LexicalAnalyzerType*)storage;
				this->m_Parser = (ParserType*)((size_t)storage + sizeof(LexicalAnalyzerType));

				// Initialize the lexical analyzer and the parser
				this->m_Pattern += REGEX_END_MARKER;
				this->m_Lexer = new(this->m_Lexer) LexicalAnalyzerType{ AUTOMATON, TOKEN_FACTORY, this->m_Pattern };
				this->m_Parser = new(this->m_Parser) ParserType{ *this->m_Lexer, PARSING_TABLE, START_SYMBOL };

				// Initialize the pattern automaton that will be used for matching string inputs
				const auto FSMData = this->m_Parser->parse<std::pair<FSMStateSetType, DFATableType>>();
				const FSMStateSetType& finalStates = FSMData.first;
				const DFATableType& transTable = FSMData.second;
				unsigned FSMFlags = 0;
				this->m_PatternAutomaton = DFAType{ finalStates, transTable, FSMFlags };
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
			std::string getFlags() const { return this->_flags_to_string(); };
		};

		struct RegularExpressionResult {
		};
	}
};
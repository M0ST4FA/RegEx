#pragma once
#include <string>

#include "LRParser.hpp"
#include "regexTypedefs.h"

namespace m0st4fa {
	

	namespace regex {

		typedef class RegularExpression regex;
		typedef struct RegularExpressionResult regexRes;

		class RegularExpression {
			// data members
			std::string m_Pattern;
			index_t m_Index = 0;
			size_t m_Flags = (size_t)Flag::F_NONE;
			/*LexicalAnalyzerType m_Lexer;
			ParserType m_Pdarser{ m_Lexer, PARSING_TABLE, START_SYMBOL };*/

			static const ParsingTableType PARSING_TABLE;
			static const SymbolType START_SYMBOL;
			static const TokenFactType TOKEN_FACTORY;
			static const DFAType AUTOMATON;
			// private methods

			static GrammarType _get_grammar();
			static ParsingTableType _get_parsing_table();
			static constexpr inline SymbolType _get_start_symbol();
			static constexpr TokenFactType _get_token_factory();
			static DFAType _get_automaton();

		public:

			RegularExpression() = default;
			RegularExpression(const std::string& pattern) : m_Pattern{ pattern } {};
			RegularExpression(const std::string& pattern, const std::string& flags) : m_Pattern{ pattern } {
				// TODO: HANDLE FLAGS
			};
			RegularExpression(const char* pattern) : m_Pattern{ pattern } {}
			RegularExpression(const char* pattern, const char* flags) : m_Pattern{ pattern } {
				// TODO: HANDLE FLAGS
				std::cout << PARSING_TABLE.grammar.toString();
			}

			// functional methods
			RegularExpressionResult exec(const std::string&);
			bool match(const std::string&);

			// getters
			std::string getPattern() { return m_Pattern; };
			index_t getIndex() { return m_Index; };
			std::string getFlags() { return std::to_string(m_Flags); }; // TODO: CREATE A SUITABLE CONVERSION METHOD TO RETURN THIS AS A STRING.

		};

		struct RegularExpressionResult {


		};
	}

};
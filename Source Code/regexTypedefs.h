#pragma once

#include "LRPGenerator.h"
#include "regexEnum.h"

namespace m0st4fa::regex {
	constexpr size_t STATE_COUNT = 30;
	constexpr size_t INPUT_COUNT = 128;
	constexpr char REGEX_END_MARKER = 127;
	using index_t = unsigned int;

	// TODO: change FSMTable to be an API implementation around vectors
	using AttributeType = std::string_view;
	using TokenType = Token<Terminal, AttributeType>;
	using InputType = std::string_view;
	using TokenFactType = TokenFactoryT<TokenType, InputType>;

	using DFATableType = FSMTable<STATE_COUNT, INPUT_COUNT>;
	using TransFnType = TransFn<DFATableType>;
	using DFAType = DFA<TransFnType, InputType>;

	using LexicalAnalyzerType = LexicalAnalyzer<TokenType, DFATableType, InputType>;

	using SymbolType = Symbol<Terminal, Variable>;
	using FirstType = std::vector<std::set<SymbolType>>;
	using ProductionType = ProductionRecord<SymbolType, LRProductionElement<SymbolType>>;
	using ItemType = Item<ProductionType>;
	using ItemSetType = ItemSet<ItemType>;

	using GrammarType = ProductionVector<ProductionType>;

	struct Position {
		size_t position = 0;
		char lexeme = '\0';
		
		explicit(false) operator std::string() const {
			return std::format("<{}, {}:{}>", position, lexeme == REGEX_END_MARKER ? '#' : lexeme, (size_t)lexeme);
		}
		bool operator==(const Position& rhs) const {
			return position	== rhs.position && lexeme == rhs.lexeme;
		};
		bool operator<(const Position& rhs) const {
			return position < rhs.position;
		};
	};
	struct DataType {
		struct Positions {
			std::set<Position> firstpos{};
			std::set<Position> lastpos{};
			bool nullable = false;

			void insertFirstpos(const std::set<Position>& positions) {
				std::cout << std::format("firstpos before insertion: {}\n", m0st4fa::toString(lastpos));
				firstpos.insert(positions.begin(), positions.end());
				std::cout << std::format("firstpos after insertion: {}\n", m0st4fa::toString(lastpos));
			}
			void insertLastpos(const std::set<Position>& positions) {
				std::cout << std::format("lastpos before insertion: {}\n", m0st4fa::toString(lastpos));
				lastpos.insert(positions.begin(), positions.end());
				std::cout << std::format("lastpos after insertion: {}\n", m0st4fa::toString(lastpos));

			}
			// in case any of these is true, the firstpos and laspost, respectively will be considered all the created positions, except the ones in `firstpos` and `lastpos` members.
			bool fpAllPositions = false;
			bool lpAllPositions = false;
		};

		Positions data;

		explicit(false) operator std::string() const {
			const auto& firstpos = data.firstpos;
			const auto& lastpos = data.lastpos;
			bool nullable = data.nullable;

			return std::format("({}, {}, {})", toString(firstpos), toString(lastpos), nullable);
		}
		explicit(false) operator bool() const {
			return data.firstpos.size() || data.lastpos.size();
		}
	};

	using StateType = LRState<DataType, TokenType>;

	using ParsingTableType = LRParsingTable<GrammarType>;
	using ParserType = LRParser<GrammarType, LexicalAnalyzerType, SymbolType, StateType, ParsingTableType>;
	using ParserGeneratorType = LRParserGenerator<GrammarType, ItemType, ParsingTableType>;

	using StackType = LRStackType<DataType, TokenType>;

	SymbolType toSymbol(const Terminal);
	SymbolType toSymbol(const Variable);
}

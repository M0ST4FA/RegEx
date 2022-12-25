#pragma once

#include "LRPGenerator.h"
#include "regexEnum.h"

namespace m0st4fa {

	namespace regex {
		constexpr size_t STATE_COUNT = 100;
		constexpr size_t INPUT_COUNT = 128;
		using index_t = unsigned int;

		// TODO: change FSMTable to be an API implementation around vectors
		using AttributeType = char;
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

		struct DataType {
			const char* data = nullptr;
			size_t size = 0;

			explicit(false) operator std::string() const {
				return data;
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


}

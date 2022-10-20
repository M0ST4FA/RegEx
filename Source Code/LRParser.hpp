#pragma once

#include "Parser.h"

namespace m0st4fa {


	template <typename GrammarT, typename LexicalAnalyzerT,
		typename SymbolT,
		typename ParsingTableT = LLParsingTable<GrammarT>, typename FSMTableT = FSMTable<>,
		typename InputT = std::string>
	class LRParser : public Parser<LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT> {

		using ParserBase = Parser<LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT>;


	public:
		// methods
		// TODO: SymbolT parse(ActionT, ExecutionOrder) => add the action parameter;

		
	};

	
}

namespace m0st4ta {
	
}
#pragma once

#include "Parser.h"

namespace m0st4fa {


	template <typename SymbolT, typename TokenT,
		typename ParsingTableT = LLParsingTable<>, typename FSMTableT = FSMTable<>,
		typename InputT = std::string>
	class LRParser : public Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT> {

		using ParserBase = Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>;


	public:
		// methods
		// TODO: SymbolT parse(ActionT, ExecutionOrder) => add the action parameter;

		
	};

	
}

namespace m0st4ta {
	
}
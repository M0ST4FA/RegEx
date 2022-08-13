#pragma once

#include "Parser.h"

namespace m0st4fa {

	template <typename SymbolT, typename TokenT,
		typename ParsingTableT, typename FSMTableT = FSMTable<50, 'z'>, // set them to have a default value
		typename InputT = std::string>
	class LRParser : public Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT> {


	public:
		// methods
		// TODO: SymbolT parse(ActionT, ExecutionOrder) => add the action parameter;

		
	};

	
}

namespace m0st4ta {
	
}
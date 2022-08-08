#pragma once

#include "Parser.h"

namespace m0st4fa {

	template <typename TableT, typename SymbolT, typename TokenT, typename InputT = std::string>
	class LRParser : public Parser<TableT, TokenT, InputT> {
		using ExecutionOrder = Parser<TableT, TokenT, InputT>::ExecutionOrder;
		using ActionT = Parser<TableT, TokenT, InputT>::ActionT;

	public:
		// methods
		// TODO: SymbolT parse(ActionT, ExecutionOrder) => add the action parameter;
		SymbolT parse(ActionT, ExecutionOrder);

		
	};

	
}

namespace m0st4ta {
	
}
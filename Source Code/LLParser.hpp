#pragma once

#include "Parser.h"

namespace m0st4fa {

	template <typename TableT, typename SymbolT, typename TokenT, typename InputT = std::string>
	class LLParser : public Parser<TableT, TokenT, InputT> {
		using ExecutionOrder = Parser<TableT, TokenT, InputT>::ExecutionOrder;

	public:

		// methods
		// TODO: SymbolT parse(ActionT, ExecutionOrder) => add the action parameter;
		SymbolT parse(ExecutionOrder);


	};


}

namespace m0st4ta {

}
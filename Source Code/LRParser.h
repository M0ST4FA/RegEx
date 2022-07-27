#pragma once

#include "Parser.h"

namespace m0st4fa {

	template <typename TableT, typename SymbolT, typename token_t, typename InputT = std::string>
	class LRParser : public Parser<TableT, token_t, InputT> {


	public:
		// enums
		enum struct ExecutionOrder : unsigned char {
			EO_PRE = 0,
			EO_IN,
			EO_POST,
			EO_NUM
		};

		// methods
		// TODO: SymbolT parse(ActionT, ExecutionOrder) => add the action parameter;
		SymbolT parse(ExecutionOrder);

		
	};

	
}
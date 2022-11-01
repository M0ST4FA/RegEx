#pragma once

#include "PEnum.h"
#include "PProduction.h"
#include "PStack.h"
#include "PTable.h"
#include "PItem.h"

namespace m0st4fa {
	

	struct ParserResult {

	};

	template<typename DataT>
	struct LRParserState {
		size_t state = SIZE_MAX;
		DataT data{};
	};
	
}

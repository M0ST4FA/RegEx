#pragma once

namespace m0st4fa {

	enum struct ExecutionOrder {
		EO_PREORDER = 0,
		EO_INORDER,
		EO_POSTORDER,
		EO_NUM
	};

	// Parser Stack
	enum StackElementType {
		SE_GRAM_SYMBOL,
		SE_SYNTH_RECORD,
		SE_ACTION_RECORD,
		SE_NUM
	};

}

#pragma once

namespace m0st4fa {

	enum struct ExecutionOrder {
		EO_PREORDER = 0,
		EO_INORDER,
		EO_POSTORDER,
		EO_NUM
	};

	enum struct ErrorRecoveryType {
		ERT_NONE = 0,
		ERT_PANIC_MODE,
		ERT_PHRASE_LEVE,
		ERT_GLOBAL,
		ERT_ABORT,
		ERT_NUM,
	};

	// Parser Stack
	enum StackElementType {
		SE_GRAM_SYMBOL,
		SE_SYNTH_RECORD,
		SE_ACTION_RECORD,
		SE_NUM
	};

}

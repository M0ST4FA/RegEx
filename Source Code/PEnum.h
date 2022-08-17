#pragma once
#include <string>
#include <ostream>

namespace m0st4fa {

	enum struct ExecutionOrder {
		EO_PREORDER = 0,
		EO_INORDER,
		EO_POSTORDER,
		EO_NUM
	};
	std::string stringfy(ExecutionOrder);
	std::ostream& operator<<(std::ostream&, ExecutionOrder);


	enum struct ErrorRecoveryType {
		ERT_NONE = 0,
		ERT_PANIC_MODE,
		ERT_PHRASE_LEVE,
		ERT_GLOBAL,
		ERT_ABORT,
		ERT_NUM,
	};
	std::string stringfy(ErrorRecoveryType);
	std::ostream& operator<<(std::ostream&, ErrorRecoveryType);

	// Parser Stack
	enum StackElementType {
		SET_GRAM_SYMBOL,
		SET_SYNTH_RECORD,
		SET_ACTION_RECORD,
		SET_NUM
	};
	std::string stringfy(StackElementType);
	std::ostream& operator<<(std::ostream&, StackElementType);

}

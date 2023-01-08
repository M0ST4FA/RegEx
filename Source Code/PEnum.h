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

	/**
	* The effect of ERT_NONE is close to ERT_PANIC_MODE because of how the parser is architected.
	* ERT_PANIC_MODE is more powerfull though: wherease ERT_NONE can only sync in case we find a token from FIRST, ERT_PANIC_MODE can do a lot more, and is not limited to this only.
	*/
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
	enum class ProdElementType {
		PET_GRAM_SYMBOL,
		PET_SYNTH_RECORD,
		PET_ACTION_RECORD,
		PET_COUNT
	};
	std::string stringfy(ProdElementType);
	std::ostream& operator<<(std::ostream&, ProdElementType);
}

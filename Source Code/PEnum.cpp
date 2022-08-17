#include "PEnum.h"

namespace m0st4fa {
	
	std::string stringfy(ExecutionOrder execOrder) {

		static constexpr const char* const names[] = {
			"PRE_ORDER",
			"IN_ORDER",
			"POST_ORDER",
		};
		
		const char* name = execOrder == ExecutionOrder::EO_NUM ? 
			std::to_string((unsigned)ExecutionOrder::EO_NUM).data() : names[static_cast<int>(execOrder)];

		return name;
	};

	std::ostream& operator<<(std::ostream& os, ExecutionOrder execOrder) {
		return os << stringfy(execOrder);
	};

	std::string stringfy(ErrorRecoveryType type) {

		static constexpr const char* const names[] = {
			"NONE",
			"PANIC_MODE",
			"PHRASE_LEVE",
			"GLOBAL",
			"ABORT",
		};
		
		const char* name = type == ErrorRecoveryType::ERT_NUM ? 
			std::to_string((unsigned)ErrorRecoveryType::ERT_NUM).data() : names[static_cast<int>(type)];

		return name;
	}
	std::ostream& operator<<(std::ostream& os, ErrorRecoveryType type) {
		return os << stringfy(type);
	};
	
	std::string stringfy(StackElementType type) {


		static constexpr const char* const names[] = {
			"GRAM_SYMBOL",
			"SYNTH_RECORD",
			"ACTION_RECORD",
		};
		

		const char* name = type == StackElementType::SET_NUM ? 
			std::to_string((unsigned)StackElementType::SET_NUM).data() : names[static_cast<int>(type)];

		return name;
	}
	std::ostream& operator<<(std::ostream& os, StackElementType type) {
		return os << stringfy(type);
	};
	
}
#include "PEnum.h"

namespace m0st4fa {
	
	std::ostream& operator<<(std::ostream& os, ExecutionOrder execOrder) {
		static constexpr const char* const names[] = {
			"PRE_ORDER",
			"IN_ORDER",
			"POST_ORDER",
		};
		
		const char* name = execOrder == ExecutionOrder::EO_NUM ? nullptr : names[static_cast<int>(execOrder)];
		
		return os << (name ? name : std::to_string((unsigned)ExecutionOrder::EO_NUM).data());
	};
	
	std::ostream& operator<<(std::ostream& os, ErrorRecoveryType type) {
		
		static constexpr const char* const names[] = {
			"NONE",
			"PANIC_MODE",
			"PHRASE_LEVE",
			"GLOBAL",
			"ABORT",
		};
		
		const char* name = type == ErrorRecoveryType::ERT_NUM ? nullptr : names[static_cast<int>(type)];
		
		return os << (name ? name : std::to_string((unsigned)ErrorRecoveryType::ERT_NUM).data());
	};

	std::ostream& operator<<(std::ostream& os, StackElementType type) {

		static constexpr const char* const names[] = {
			"GRAM_SYMBOL",
			"SYNTH_RECORD",
			"ACTION_RECORD",
		};
		

		const char* name = type == StackElementType::SET_NUM ? nullptr : names[static_cast<int>(type)];

		return os << (name ? name : std::to_string((unsigned)StackElementType::SET_NUM).data());
	};
	
}
#pragma once
#include <string>


namespace m0st4fa {
	// TODO: error subsystem.

	enum class ERROR_TYPE {
		ET_INVALID_ARGUMENT,
		ET_ERROR_TYPE_MAX
	};
	

	class ErrorReporter {
	
		static constexpr const char* ERROR_TYPE_NAMES[(int)ERROR_TYPE::ET_ERROR_TYPE_MAX] = {
			"Invalid Argument",
		};
		
	public:
		void report(ERROR_TYPE, const std::string&);
		void report(ERROR_TYPE, const std::string&, const std::string&);
		
	};
	
}

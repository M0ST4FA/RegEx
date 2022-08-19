#pragma once

#include <iostream>
#include <cstdarg>
#include <format>
#include <source_location>

//#define _TRACE

namespace m0st4fa {
	
	// DECLARATIONS
	enum class ERROR_TYPE {
		ET_INVALID_ARGUMENT,
		ET_INVALID_LEXEME,
		ET_PROD_BODY_EMPTY,
		ET_UNEXCPECTED_TOKEN,
		ET_ERROR_TYPE_MAX
	};

	enum class LOG_LEVEL {
		LL_FATAL_ERROR,
		LL_ERROR,
		LL_WARRNING,
		LL_INFO,
		LL_DEBUG,
		LL_LOG_LEVEL_MAX
	};

	struct LoggerInfo {
		LOG_LEVEL level;
		union {
			int noVal = 0;
			ERROR_TYPE errorType;
		} info;
	};
	
	class Logger {

		static constexpr const volatile char* LOG_LEVEL_STRING[(int)LOG_LEVEL::LL_LOG_LEVEL_MAX] = {
			"FATAL ERROR",
			"ERROR",
			"WARNING",
			"INFO",
			"DEBUG",
		};

		static constexpr const volatile char* ERROR_TYPE_NAMES[(int)ERROR_TYPE::ET_ERROR_TYPE_MAX] = {
			"Invalid Argument",
			"Invlaid Lexeme",
			"Empty Production Body",
			"Unexpected Token"
		};
		
	public:
		
		void log(const LoggerInfo, const std::string&, std::source_location = std::source_location::current()) const;
		
		void logDebug(const std::string&, std::source_location = std::source_location::current()) const;

	};


}

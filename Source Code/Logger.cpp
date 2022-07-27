
#include "Logger.h"

namespace m0st4fa {
	
	void Logger::log(LoggerInfo loggerInfo, const std::string& message, std::source_location location) const
	{

		// if logging a debug message
		if (loggerInfo.level == LOG_LEVEL::LL_DEBUG) {
			logDebug(message, location);
			return;
		};

		// if logging any other message
		const std::string logLevelStr = LOG_LEVEL_STRING[static_cast<int>(loggerInfo.level)];
		const std::string errorTypeStr = ERROR_TYPE_NAMES[static_cast<int>(loggerInfo.info.errorType)];

		std::string messageStr;
		

		if (loggerInfo.level == LOG_LEVEL::LL_ERROR || loggerInfo.level == LOG_LEVEL::LL_FATAL_ERROR)
			messageStr += std::format("[{:s} => {:s}]: {:s}\n", logLevelStr, errorTypeStr, message);
		else
			messageStr += std::format("[{:s}]: {:s}\n", logLevelStr, message);
#ifdef _DEBUG 
#ifdef _TRACE
		messageStr += std::string("File Name: ") + location.file_name() + std::string("\n");
		messageStr += std::string("Line: ") + std::to_string(location.line()) + std::string(", Character: ") + std::to_string(location.column()) + "\n";
		messageStr += std::string("Function: ") + location.function_name() + "\n";
#endif
#endif

		switch (loggerInfo.level) {
		case LOG_LEVEL::LL_FATAL_ERROR:
		case LOG_LEVEL::LL_ERROR:
			std::cerr << messageStr << "\n";
			return;
		case LOG_LEVEL::LL_WARRNING:
			std::cout << messageStr << "\n";
			return;
		case LOG_LEVEL::LL_INFO:
			std::clog << messageStr << "\n";
			return;
		default:
			std::cerr << "Unknown log level: " << static_cast<int>(loggerInfo.level) << "\n";
			throw std::invalid_argument("Unknown log level.");
		};

	}

	void Logger::logDebug(const std::string& message, std::source_location location) const
	{

#ifdef _DEBUG
		const std::string logLevelStr = LOG_LEVEL_STRING[static_cast<int>(LOG_LEVEL::LL_DEBUG)];
		std::string messageStr = std::format("[{:s}]: {:s}", logLevelStr, message);
		
#ifdef _TRACE
		messageStr += std::string("File Name: ") + location.file_name() + std::string("\n");
		messageStr += std::string("Line: ") + std::to_string(location.line()) + std::string(", Character: ") + std::to_string(location.column()) + "\n";
		messageStr += std::string("Function: ") + location.function_name() + "\n";
#endif
		
		std::cout << messageStr << "\n";
#endif

		return;
	}

	
}

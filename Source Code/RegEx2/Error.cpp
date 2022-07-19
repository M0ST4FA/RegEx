#include "Error.h"
#include <format>
#include <iostream>

void m0st4fa::ErrorReporter::report(ERROR_TYPE type, const std::string& message)
{
	const std::string errorType = ERROR_TYPE_NAMES[(int)type];
	std::string output = std::format("{:s}:\n{:=>{}c}\n* {:<s}\n", errorType, '\0', errorType.length() + 2, message);
	
	std::cerr << output;
	return;
};

void m0st4fa::ErrorReporter::report(ERROR_TYPE type, const std::string& cause, const std::string& suggestion)
{
	const std::string errorType = ERROR_TYPE_NAMES[(int)type];
	std::string output = std::format("{:s}:\n{:=>{}c}\n* {:<s}\n* {:<s}\n", errorType, '\0', errorType.length() + 2, cause, suggestion);

	std::cerr << output;
	return;
};

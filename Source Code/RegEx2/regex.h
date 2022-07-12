#pragma once
#include <string>


using regex = class RegularExpression;
using regexRes = struct RegularExpressionResult;
using flag_t = unsigned int;
using index_t = unsigned int;


class RegularExpression {
	// data members
	std::string m_Source;
	index_t m_Index;
	flag_t m_Flags;
	
	// private methods
	

public:
	// functional methods
	RegularExpressionResult exec(const std::string&);
	bool match(const std::string&);
	
	// getters
	std::string getSource() { return m_Source; };
	index_t getIndex() { return m_Index; };
	flag_t getFlags() { return m_Flags; };
	
};


struct RegularExpressionResult {

	
};


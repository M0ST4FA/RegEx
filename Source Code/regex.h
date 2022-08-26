#pragma once
#include <string>

#include "LRParser.hpp"


namespace m0st4fa {
	
	typedef class RegularExpression regex;
	typedef struct RegularExpressionResult regexRes;
	using flag_t = unsigned int;
	using index_t = unsigned int;


	class RegularExpression {
		// data members
		std::string m_Source;
		index_t m_Index;
		flag_t m_Flags;
		LRParser<size_t, size_t, size_t, size_t> m_Parser;

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

};
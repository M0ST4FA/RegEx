#pragma once

#include "LexicalAnalyzer.h"

namespace m0st4fa {
	

	template <typename TableT, typename token_t, typename input_t = std::string>
	class Parser {
		LexicalAnalyzer<token_t, input_t> m_LexicalAnalyzer;
		TableT m_Table;

	protected:
		LexicalAnalyzer<token_t, input_t>& getLexicalAnalyzer() { return m_LexicalAnalyzer; };
		TableT& getTable() { return m_Table; };

	public:
				

	};

	
}

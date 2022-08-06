#pragma once

#include "LexicalAnalyzer.h"

namespace m0st4fa {
	

	template <typename TableT, typename TokenT, typename InputT = std::string>
	class Parser {
		LexicalAnalyzer<TokenT, InputT> m_LexicalAnalyzer;
		TableT m_Table;

	protected:
		LexicalAnalyzer<TokenT, InputT>& getLexicalAnalyzer() { return m_LexicalAnalyzer; };
		TableT& getTable() { return m_Table; };

	public:

		// enums
		static enum struct ExecutionOrder : unsigned char {
			EO_PREORDER = 0,
			EO_INORDER,
			EO_POSTORDER,
			EO_NUM
		};

		

	};

	
}

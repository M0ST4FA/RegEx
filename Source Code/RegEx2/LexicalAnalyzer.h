#pragma once

#include "DFA.h"

#include <string>


namespace m0st4fa {
	
	
	template <typename token_t, typename input_t = std::string>
	class LexicalAnalyzer {
		using TokenFactoryT = token_t (*)(state_t, input_t);

		DFA<TransFn<input_t>, input_t> automaton;
		TokenFactoryT m_TokenFactory = nullptr;

	public:
		
		
	};

	
}
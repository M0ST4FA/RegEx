#pragma once

#include "DFA.h"

#include <string>


namespace m0st4fa {
	
	
	template <typename token_t, typename InputT = std::string>
	class LexicalAnalyzer {
		using TokenFactoryT = token_t (*)(state_t, InputT);

		DFA<TransFn<InputT>, InputT> automaton;
		TokenFactoryT m_TokenFactory = nullptr;

	public:
		
		
	};

	
}
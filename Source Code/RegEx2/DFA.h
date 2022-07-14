#pragma once

#include "FiniteStateMachine.h"

namespace m0st4fa {

	template <typename TransFuncT, typename input_t = std::string>
	class DeterFiniteAutomata {
		using set_t = std::set<state_t>;
		// fields

		// static variables
		constexpr static state_t m_DeadState = -1;

	public:
		DeterFiniteAutomata() = default;
		DeterFiniteAutomata(const set_t& fStates, TransFuncT& tranFn, flag_t flags) :
			FiniteStateMachine<TransFuncT, input_t> {fStates, tranFn, flags}
		{};

		
	};

	template <typename TransFuncT, typename input_t = std::string>
	using DFA = DeterFiniteAutomata<TransFuncT, input_t>;
	
}
#pragma once

#include "FiniteStateMachine.h"

namespace m0st4fa {

	// DECLARATIONS
	template <typename TransFuncT, typename InputT = std::string>
	class DeterFiniteAutomata : FiniteStateMachine<TransFuncT, InputT> {
		using set_t = std::set<state_t>;
		// fields

		// static variables
		constexpr static state_t DEAD_STATE = 0;

		// private methods
		FSMResult _simulate_whole_string(const InputT&) const;
		FSMResult _simulate_longest_prefix(const InputT&) const;
		
		/**
		* @return (accepted?, endIndex)
		*/
		bool _check_accepted_longest_prefix(const std::vector<state_t>&, size_t&) const;
		
	public:
		DeterFiniteAutomata() = default;
		DeterFiniteAutomata(const set_t& fStates, const TransFuncT& tranFn, flag_t flags) :
			FiniteStateMachine<TransFuncT, InputT> {fStates, tranFn, flags}
		{};

		FSMResult simulate(const InputT&, FSM_MODE) const;
		
	};

	template <typename TransFuncT, typename InputT = std::string>
	using DFA = DeterFiniteAutomata<TransFuncT, InputT>;
	
	
	

	// IMPLEMENTATIONS
	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomata<TransFuncT, InputT>::_simulate_whole_string(const InputT& input) const
	{
		state_t currState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Break if you hit a dead state since it is dead.
		*/
		for (auto c : input) {
			currState = this->m_TransitionFunc(currState, c);

			if (currState == DEAD_STATE)
				break;
		}

		bool accepted = this->getFinalStates().contains(currState);

		return FSMResult(accepted, { 0, accepted ? input.size() : 0}, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomata<TransFuncT, InputT>::_simulate_longest_prefix(const InputT& input) const
	{
		state_t currState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/ 
		std::vector matchedStates = { currState };
		size_t index = 0;

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		 * Break if you hit a dead state since it is dead.
		*/
		for (auto c : input) {
			// get next state
			currState = this->m_TransitionFunc(currState, c);
			index++;

			// break out if it is dead
			if (currState == DEAD_STATE)
				break;

			// update our path through the machine
			matchedStates.push_back(currState);
		}

		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStates, index);

		return FSMResult(accepted, { 0, index }, input);
	}
	
	template<typename TransFuncT, typename InputT>
	inline bool DeterFiniteAutomata<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<state_t>& matchedStates, size_t& index) const
	{
		bool accepted = false;

		/**
		* Loop through the path from the end seeking the closes final state.
		* Update the end index as you do so.
		*/
		auto it = matchedStates.rbegin();
		while (it != matchedStates.rend())
		{
			state_t currState = *it;
			if (this->getFinalStates().contains(currState)) {
				accepted = true;
				break;
			}

			index--;
			it++;
		}
		
		return accepted;
	};

	template<typename TransFuncT, typename InputT>
	inline FSMResult DeterFiniteAutomata<TransFuncT, InputT>::simulate(const InputT& input, FSM_MODE mode) const
	{
		switch (mode) {
		case FSM_MODE::MM_WHOLE_STRING:
			return this->_simulate_whole_string(input);
		case FSM_MODE::MM_LONGEST_PREFIX:
			return this->_simulate_longest_prefix(input);
		default:
			std::cerr << "Unreachable: simulate() cannot reach this point." << std::endl;
		}

	}

}
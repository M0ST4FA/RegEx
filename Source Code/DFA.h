#pragma once

#include "FiniteStateMachine.h"

namespace m0st4fa {

	// DECLARATIONS
	/**
	* @brief A DFA that that can be used to match strings.
	* Provides a single function: simulate();
	*/
	template <typename TransFuncT, typename InputT = std::string>
	class DeterFiniteAutomatan: FiniteStateMachine<TransFuncT, InputT> {
		// fields

		// static variables
		constexpr static state_t DEAD_STATE = 0;

		// private methods
		FSMResult _simulate_whole_string(const InputT&) const;
		FSMResult _simulate_longest_prefix(const InputT&) const;
		FSMResult _simulate_longest_substring(const InputT&) const;

		bool _check_accepted_longest_prefix(const std::vector<state_t>&, size_t&) const;
		
	public:
		DeterFiniteAutomatan() = default;
		DeterFiniteAutomatan(const state_set_t& fStates, const TransFuncT& tranFn, flag_t flags = FSM_FLAG::FF_FLAG_MAX) :
			FiniteStateMachine<TransFuncT, InputT> {fStates, tranFn, FSM_TYPE::MT_DFA, flags}
		{};

		FSMResult simulate(const InputT&, FSM_MODE) const;
		
	};

	template <typename TransFuncT, typename InputT = std::string>
	using DFA = DeterFiniteAutomatan<TransFuncT, InputT>;
	

	// IMPLEMENTATIONS
	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_whole_string(const InputT& input) const
	{
		state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		state_t currState = startState;

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

		return FSMResult(accepted, accepted ? state_set_t{currState} : state_set_t{startState}, { 0, accepted ? input.size() : 0 }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_prefix(const InputT& input) const
	{
		state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		state_t currState = startState;
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

		return FSMResult(accepted, accepted ? state_set_t{ matchedStates.at(index)} : state_set_t{startState}, {0, index}, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_substring(const InputT& input) const
	{
		state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		state_t currState = startState;
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		std::vector matchedStates = { currState };
		size_t startIndex = 0, endIndex = 0;

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		 * Break if you hit a dead state since it is dead.
		 * If a substring was not accepted, start matching the next substring if any
		*/
		for (; startIndex < input.size(); endIndex = ++startIndex) {
			// start from startIndex until the end of the string or until you reach a dead state
			/**
			* The value of endIndex will be correct because the last time we increment it, the condition is guaranteed to fail.
			* This guarantees that endIndex will always equal the size of the string - startIndex.
			*/
			for (; endIndex < input.size(); endIndex++) {
				// get next state
				auto c = input[endIndex];
				currState = this->m_TransitionFunc(currState, c);

				// break out if it is dead
				if (currState == DEAD_STATE)
					break;

				// update our path through the machine
				matchedStates.push_back(currState);
			};

			// figure out whether there is an accepted longest prefix
			bool accepted = _check_accepted_longest_prefix(matchedStates, endIndex);

			// if this substring was not accepted, reset the current state and loopback
			if (-not accepted) {
				currState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
				continue;
			}

			// if it was accepted:
			typedef unsigned long ull;
			return FSMResult{ true, state_set_t { matchedStates.at(endIndex) }, {(ull)startIndex, (ull)endIndex}, input };
		}

		return FSMResult(false, state_set_t {startState}, {0, 0}, input);
	}
	
	template<typename TransFuncT, typename InputT>
	inline bool DeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<state_t>& matchedStates, size_t& index) const
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

			index = FiniteStateMachine<TransFuncT, InputT>::START_STATE ? 0 : index - 1;
			it++;
		}
		
		return accepted;
	};

	/**
	* @brief Simulate the given input string using the given simulation method.
	*/
	template<typename TransFuncT, typename InputT>
	inline FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::simulate(const InputT& input, FSM_MODE mode) const
	{
		switch (mode) {
		case FSM_MODE::MM_WHOLE_STRING:
			return this->_simulate_whole_string(input);
		case FSM_MODE::MM_LONGEST_PREFIX:
			return this->_simulate_longest_prefix(input);
		case FSM_MODE::MM_LONGEST_SUBSTRING:
			return this->_simulate_longest_substring(input);
		default:
			std::cerr << "Unreachable: simulate() cannot reach this point." << std::endl;
			// TODO: throw a better exception
			throw std::runtime_error("The provided mode is erroneous in function DFA::simulate().");
		}

	}

}
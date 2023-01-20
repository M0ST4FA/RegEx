#pragma once

#include "FiniteStateMachine.h"
#include <assert.h>

namespace m0st4fa {

	// DECLARATIONS
	/**
	* @brief A DFA that that can be used to match strings.
	* Provides a single function: simulate();
	*/
	template <typename TransFuncT, typename InputT = std::string_view>
	class DeterFiniteAutomatan: protected FiniteStateMachine<TransFuncT, InputT> {
		// fields

		using Base = FiniteStateMachine<TransFuncT, InputT>;
		// static variables
		constexpr static state_t DEAD_STATE = 0;

		// private methods
		FSMResult _simulate_whole_string(const InputT&) const;
		FSMResult _simulate_longest_prefix(const InputT&) const;
		FSMResult _simulate_longest_substring(const InputT&) const;

		bool _check_accepted_longest_prefix(const std::vector<state_t>&, size_t&) const;
		bool _check_accepted_substring(const InputT&, std::vector<state_t>&, size_t, size_t&) const;
		
	public:
		DeterFiniteAutomatan() = default;
		DeterFiniteAutomatan(const FSMStateSetType& fStates, const TransFuncT& tranFn, flag_t flags = FSM_FLAG::FF_FLAG_MAX) :
			FiniteStateMachine<TransFuncT, InputT> {fStates, tranFn, FSM_TYPE::MT_DFA, flags}
		{};
		DeterFiniteAutomatan& operator=(const DeterFiniteAutomatan& rhs) {
			this->Base::operator=(rhs);
			return *this;
		}

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

		return FSMResult(accepted, accepted ? FSMStateSetType{currState} : FSMStateSetType{startState}, { 0, accepted ? input.size() : 0 }, input);
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

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		 * Break if you hit a dead state since it is dead.
		*/
		for (auto c : input) {
			// get next state
			currState = this->m_TransitionFunc(currState, c);

			// break out if it is dead
			if (currState == DEAD_STATE)
				break;

			// update our path through the machine
			matchedStates.push_back(currState);
		}

		size_t index = matchedStates.size();
	 	this->m_Logger.logDebug(std::format("[DFA] index of the last checked character: {}\n[DFA] size of matchedStates: {}\n", index, matchedStates.size()));
		//assert(index == matchedStates.size());
		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStates, index);

#if defined(_DEBUG)
		std::cout << "[DFA] matched set of states (path through the FSM): " << matchedStates;
#endif

		return FSMResult(accepted, accepted ? FSMStateSetType{ matchedStates.at(index) } : FSMStateSetType{startState}, {0, index}, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult DeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_substring(const InputT& input) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		std::vector matchedStates = { startState };
		size_t startIndex = 0;
		size_t charIndex = 0;

		/**
		 * Follow a path through the machine using the characters of the string until you check all the characters.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		 * Break if you hit a dead state since it is dead.
		 * If a substring was not accepted, start matching the next substring if any
		*/
		for (; startIndex < input.size(); charIndex = ++startIndex) {
			
			// if this substring was not accepted
			if (bool accepted = _check_accepted_substring(input, matchedStates, startIndex, charIndex);
				!accepted)
				continue;

			// if it was accepted:
			using ull = unsigned long;
			return FSMResult{ true, FSMStateSetType { matchedStates.at(charIndex - startIndex) }, {(ull)startIndex, (ull)charIndex}, input };
		}

		return FSMResult(false, FSMStateSetType {startState}, {0, 0}, input);
	}
	
	template<typename TransFuncT, typename InputT>
	inline bool DeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<state_t>& matchedStates, size_t& index) const
	{
		bool accepted = false;
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;

		/**
		* Loop through the path from the end seeking the closes final state.
		* Update the end index as you do so.
		* The number of states in the set is: (the number of characters in the longest prefix + 1, the start state).
		* The last state in the matchedSet is almost guaranteed failed state.
		*/
		auto it = matchedStates.rbegin();
		while (it != matchedStates.rend())
		{
			state_t currState = *it;
			
			// if the current state is the start state
			if (currState == startState) {
				break;
			}

			// set up the iterator and the index for the next iteration
			index--;
			it++;

			// if the current state is a final state
			if (this->getFinalStates().contains(currState)) {
				accepted = true;
				break;
			}

		}
		
		return accepted;
	}

	template<typename TransFuncT, typename InputT>
	bool DeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_substring(const InputT& input, std::vector<state_t>& matchedStates, size_t startIndex, size_t& charIndex) const
	{
		state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		state_t currState = startState;
		// start from startIndex until the end of the string or until you reach a dead state
			/**
			* The value of charIndex will be correct because the last time we increment it, the condition is guaranteed to fail.
			* This guarantees that charIndex will always equal: the number of matched characters + startIndex.
			*/

		for (; charIndex < input.size(); charIndex++) {
			// get next state
			auto c = input[charIndex];
			currState = this->m_TransitionFunc(currState, c);

			// break out if it is dead
			if (currState == DEAD_STATE)
				break;

			// update the path through the machine
			matchedStates.push_back(currState);
		};

		// endIndex = number of characters checked + the offset of the substring into the input string
		size_t endIndex = matchedStates.size() + startIndex;
		//this->m_Logger.logDebug(std::format("at _check_accepted_substring: charIndex: {}, endIndex: {}\n", charIndex, endIndex));

		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStates, endIndex);
		
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
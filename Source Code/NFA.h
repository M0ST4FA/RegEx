#pragma once

#include "FiniteStateMachine.h"
#include <stack>
#include <functional>

namespace m0st4fa {

	// DECLARATIONS
	/**
	* @brief An NFA that that can be used to match strings.
	* The transition function must map states and input to sets of states.
	*/
	template <typename TransFuncT, typename InputT = std::string>
	class NonDeterFiniteAutomatan : FiniteStateMachine<TransFuncT, InputT> {
		// static variables
		constexpr static state_t DEAD_STATE = 0;

		// private methods
		FSMResult _simulate_whole_string(const InputT&) const;
		FSMResult _simulate_longest_prefix(const InputT&) const;
		FSMResult _simulate_longest_substring(const InputT&) const;

		bool _check_accepted_longest_prefix(const std::vector<state_set_t>&, size_t&) const;
		state_set_t _epsilon_closure(const state_set_t&) const;

	public:
		NonDeterFiniteAutomatan() = default;
		NonDeterFiniteAutomatan(const state_set_t& fStates, const TransFuncT& tranFn, FSM_TYPE machineType, flag_t flags = FSM_FLAG::FF_FLAG_MAX) :
			FiniteStateMachine<TransFuncT, InputT>{ fStates, tranFn, machineType, flags }
		{


			LoggerInfo loggerInfo = {
			  .level = LOG_LEVEL::LL_ERROR,
			  .info = {.errorType = ERROR_TYPE::ET_INVALID_ARGUMENT}
			};

			// if the correct machine type is not passed
			if (machineType != FSM_TYPE::MT_EPSILON_NFA && machineType != FSM_TYPE::MT_NON_EPSILON_NFA) {
				const std::string message = R"(NonDeterFiniteAutomatan: machineType must be either "MT_EPSILON_NFA" or "MT_NON_EPSILON_NFA")";
				this->m_Logger.log(loggerInfo, message);
				throw std::invalid_argument(message);
			};

		};


		FSMResult simulate(const InputT&, FSM_MODE) const;

	};

	template <typename TransFuncT, typename InputT = std::string>
	using NFA = NonDeterFiniteAutomatan<TransFuncT, InputT>;

	// IMPLEMENTATIONS
	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_whole_string(const InputT& input) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		state_set_t currState = { startState };

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Break if you hit a dead state since it is dead.
		*/
		if (this->getMachineType() == FSM_TYPE::MT_NON_EPSILON_NFA) // If the machine is a non-epsilon NFA
			for (auto c : input)
				currState = this->m_TransitionFunc(currState, c);
		else // If the machine is an epsilon NFA
			for (auto c : input)
				currState = _epsilon_closure(this->m_TransitionFunc(currState, c));
		
		bool accepted = false;
		const state_set_t& finalStates = this->getFinalStates();

		// assert whether we've reached a final state
		state_set_t finalState;
		auto lambda = [&finalState, &finalStates](state_t s) {
			if (finalStates.contains(s))
				finalState.insert(s);
		};


		if (accepted)
			std::for_each(currState.begin(), currState.end(), lambda);
		else
			finalState = state_set_t{ startState };
		

		return FSMResult(accepted, finalState, { 0, accepted ? input.size() : 0 }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_prefix(const InputT& input) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		std::vector<state_set_t> matchedStatesSet = { {startState} };

		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		size_t index = 0;

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		*/
		for (auto c : input) {
			index++;

			// get next set of states
			// update our path through the machine
			matchedStatesSet.push_back(this->m_TransitionFunc(matchedStatesSet.back(), c));
		}

		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStatesSet, index);
		
		const state_set_t& finalStates = this->getFinalStates();

		// assert whether we've reached a final state
		state_set_t finalState;
		auto lambda = [&finalState, &finalStates](state_t s) {
			if (finalStates.contains(s))
				finalState.insert(s);
		};

		auto currState = matchedStatesSet.at(index);
		if (accepted)
			std::for_each(currState.begin(), currState.end(), lambda);
		else
			finalState = state_set_t{ startState };

		return FSMResult(accepted, finalState, { 0, index }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_substring(const InputT& input) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		std::vector<state_set_t> matchedStatesSet = { {startState} };

		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		size_t startIndex = 0, endIndex = 0;

		for (; endIndex < input.size(); endIndex = ++startIndex ) {

			/**
			 * Follow a path through the machine using the characters of the string.
			 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
			*/
			if (this->getMachineType() == FSM_TYPE::MT_NON_EPSILON_NFA)
				for (; endIndex < input.size(); endIndex++)
				{
					this->m_Logger.logDebug(std::string() + input[endIndex]);
					auto c = input[endIndex];
					state_set_t currStateSet = this->m_TransitionFunc(matchedStatesSet.back(), c);
					
					// get next set of states
					// update our path through the machine
					matchedStatesSet.push_back(currStateSet);
				}
			else 
				for (; endIndex < input.size(); endIndex++)
				{
					auto c = input[endIndex];
					auto currStateSet = _epsilon_closure(
						this->m_TransitionFunc(matchedStatesSet.back(), c));
					
					this->m_Logger.logDebug(std::string("Set size: ") + std::to_string(currStateSet.size()));

					// get next set of states
					// update our path through the machine
					matchedStatesSet.push_back(currStateSet);
				}

			// figure out whether there is an accepted longest prefix
			bool accepted = _check_accepted_longest_prefix(matchedStatesSet, endIndex);

			// if this substring was not accepted, check the next
			if (-not accepted) {
				matchedStatesSet = { {FiniteStateMachine<TransFuncT, InputT>::START_STATE} };
				continue;
			}

			// if it was accepted

			// assert whether we've reached a final state
			const state_set_t& finalStates = this->getFinalStates();
			state_set_t finalState;
			auto lambda = [&finalState, &finalStates](state_t s) {
				if (finalStates.contains(s))
					finalState.insert(s);
			};

			auto currState = matchedStatesSet.at(endIndex);
			if (accepted)
				std::for_each(currState.begin(), currState.end(), lambda);
			else
				finalState = state_set_t{ startState };

			return FSMResult(true, finalState, { startIndex, endIndex }, input);
		}

		// if there was no accepted substring
		return FSMResult(false, {startState}, {0, 0}, input);

	}

	template<typename TransFuncT, typename InputT>
	bool NonDeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<state_set_t>& stateSet, size_t& index) const
	{
		bool res = false;
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		/**
		* Loop through the path from the end seeking the closes final state.
		* Update the end index as you do so.
		*/
		auto it = stateSet.rbegin();
		while (it != stateSet.rend())
		{
			state_set_t currStateSet = *it;

			if (currStateSet.contains(startState))
				break;
			
			// print the currently searched index (previous index)
			this->m_Logger.logDebug(
				"_check_accepted_longest_prefix: current searched index (previous index): " 
				+ 
				std::to_string(index));

			// update the index and the iterators
			index--;
			it++;

			// perform the search
			for (auto s : currStateSet)
				if (this->getFinalStates().contains(s))
				{
					res = true;
					break;
				}

			// if a final state is found, break out of the loop
			if (res)
				break;

		}
		
		return res;
	}

	template<typename TransFuncT, typename InputT>
	state_set_t NonDeterFiniteAutomatan<TransFuncT, InputT>::_epsilon_closure(const state_set_t& set) const
	{
		state_set_t res{set};

		std::stack<state_t> stack{};
		// initialize the stack
		for (auto s : set)
			stack.push(s);
		

		while (stack.size()) {
			state_t s = stack.top();
			stack.pop();

			// check if the state has
			state_set_t epsilonTransitions = this->m_TransitionFunc(s, '\0');

			if (epsilonTransitions.size() > 0) {
				/**
				* Push all the states in the epsilon transitions onto the stack.
				* We do this to consider whether the state itself has any epsilon transitions.
				* This applies the recursivness of the algorithm.
				* Before we push a state, we check to see if it is already in the set so that we don't consider the state again.
				* If we don't do that, we might end up with an infinite loop.
				*/ 
				for (auto state : epsilonTransitions)
					if (!res.contains(state))
						stack.push(state);

				res.insert(epsilonTransitions.begin(), epsilonTransitions.end());
			};

			
		};
		

		return res;
	}

	
	template<typename TransFuncT, typename InputT>
	inline FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::simulate(const InputT& input, FSM_MODE mode) const
	{
		switch (mode) {
		case FSM_MODE::MM_WHOLE_STRING:
			return this->_simulate_whole_string(input);
		case FSM_MODE::MM_LONGEST_PREFIX:
			return this->_simulate_longest_prefix(input);
		case FSM_MODE::MM_LONGEST_SUBSTRING:
			return this->_simulate_longest_substring(input);
		default:
			LoggerInfo loggerInfo = {
				  .level = LOG_LEVEL::LL_ERROR,
				  .info = {.errorType = ERROR_TYPE::ET_INVALID_ARGUMENT}
			};
			this->m_Logger.log(loggerInfo, "Unreachable: simulate() cannot reach this point. The provided mode is probably erraneous.");
			throw std::runtime_error("The provided mode is erroneous in function DFA::simulate().");
		}

	}
	
}

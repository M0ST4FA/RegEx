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

		bool _check_accepted_longest_prefix(const std::vector<FSMStateSetType>&, size_t&) const;
		bool _check_accepted_substring(const InputT&, std::vector<FSMStateSetType>&, size_t, size_t&) const;
		FSMStateSetType _epsilon_closure(const FSMStateSetType&) const;
		FSMStateSetType _get_final_states(const FSMStateSetType&) const;

	public:
		NonDeterFiniteAutomatan() = default;
		NonDeterFiniteAutomatan(const FSMStateSetType& fStates, const TransFuncT& tranFn, FSM_TYPE machineType, flag_t flags = FSM_FLAG::FF_FLAG_MAX) :
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
		FSMStateSetType currState = { startState };

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
		const FSMStateSetType& finalStates = this->getFinalStates();

		// assert whether we've reached a final state
		FSMStateSetType finalState;
		auto lambda = [&finalState, &finalStates](state_t s) {
			if (finalStates.contains(s))
				finalState.insert(s);
		};


		if (accepted)
			std::for_each(currState.begin(), currState.end(), lambda);
		else
			finalState = FSMStateSetType{ startState };
		

		return FSMResult(accepted, finalState, { 0, accepted ? input.size() : 0 }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_prefix(const InputT& input) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		std::vector<FSMStateSetType> matchedStatesSet = { {startState} };

		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/

		/**
		 * Follow a path through the machine using the characters of the string.
		 * Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		*/
		if (this->getMachineType() == FSM_TYPE::MT_NON_EPSILON_NFA)
			for (auto c : input)
				// get next set of states and update our path through the machine
				matchedStatesSet.push_back(this->m_TransitionFunc(matchedStatesSet.back(), c));
		else 
			for (auto c : input)
				// get next set of states and update our path through the machine
				matchedStatesSet.push_back(this->_epsilon_closure(this->m_TransitionFunc(matchedStatesSet.back(), c)));

		// set the index
		size_t index = matchedStatesSet.size();

		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStatesSet, index);
		
		const FSMStateSetType& finalStates = this->getFinalStates();

		// assert whether we've reached a final state
		FSMStateSetType finalState;
		auto lambda = [&finalState, &finalStates](state_t s) {
			if (finalStates.contains(s))
				finalState.insert(s);
		};

		auto currState = matchedStatesSet.at(index);
		if (accepted)
			std::for_each(currState.begin(), currState.end(), lambda);
		else
			finalState = FSMStateSetType{ startState };

		return FSMResult(accepted, finalState, { 0, index }, input);
	}

	template<typename TransFuncT, typename InputT>
	FSMResult NonDeterFiniteAutomatan<TransFuncT, InputT>::_simulate_longest_substring(const InputT& input) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;
		std::vector<FSMStateSetType> matchedStatesSet = { {startState} };
		/**
		* keeps track of the path taken through the machine.
		* Will be used to figure out the longest matched prefix, if any.
		*/
		size_t startIndex = 0, charIndex = 0;
		size_t endIndex = 0;

		for (; charIndex < input.size(); charIndex = ++startIndex ) {

			// check whether the substring is accepted
			bool accepted = _check_accepted_substring(input, matchedStatesSet, startIndex, endIndex);

			// if this substring was not accepted, check the next
			if (-not accepted)
				continue;
			
			// get the final states we've reached
			auto& currState = matchedStatesSet.at(endIndex - 1 - startIndex);
		 	FSMStateSetType finalStateSet = _get_final_states(currState);
			assert("This set must contain at least a single final state" && finalStateSet.size());

			return FSMResult(true, finalStateSet, { startIndex, endIndex }, input);
		}

		// if there was no accepted substring
		return FSMResult(false, {startState}, {0, 0}, input);

	}

	template<typename TransFuncT, typename InputT>
	bool NonDeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_longest_prefix(const std::vector<FSMStateSetType>& stateSet, size_t& index) const
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
			FSMStateSetType currStateSet = *it;
			
			//// print the currently searched index (previous index)
			//this->m_Logger.logDebug(
			//	"_check_accepted_longest_prefix: current searched index (previous index): " 
			//	+ 
			//	std::to_string(index));

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
	bool NonDeterFiniteAutomatan<TransFuncT, InputT>::_check_accepted_substring(const InputT& input, std::vector<FSMStateSetType>& matchedStatesSet, size_t startIndex, size_t& endIndex) const	
	{

		/**
			* Follow a path through the machine using the characters of the string.
			* Keep track of that path in order to be able to find the longest prefix if the whole string is not accepted.
		*/
		size_t charIndex = startIndex;
		if (this->getMachineType() == FSM_TYPE::MT_NON_EPSILON_NFA)
			for (; charIndex < input.size(); charIndex++)
			{
				auto c = input[charIndex];
				FSMStateSetType currStateSet = this->m_TransitionFunc(matchedStatesSet.back(), c);

				// if the current state is empty
				if (-not currStateSet.size())
					break;

				// get next set of states
				// update our path through the machine
				matchedStatesSet.push_back(currStateSet);
			}
		else
			for (; charIndex < input.size(); charIndex++)
			{
				auto c = input[charIndex];
				auto currStateSet = _epsilon_closure(
					this->m_TransitionFunc(matchedStatesSet.back(), c)
				);

				// if the current state is empty
				if (-not currStateSet.size())
					break;

				// get next set of states
				// update our path through the machine
				matchedStatesSet.push_back(currStateSet);
			}

		endIndex = matchedStatesSet.size() + startIndex;
		// figure out whether there is an accepted longest prefix
		bool accepted = _check_accepted_longest_prefix(matchedStatesSet, endIndex);

		return accepted;
	}

	template<typename TransFuncT, typename InputT>
	FSMStateSetType NonDeterFiniteAutomatan<TransFuncT, InputT>::_epsilon_closure(const FSMStateSetType& set) const
	{
		FSMStateSetType res{set};

		std::stack<state_t> stack{};
		// initialize the stack
		for (auto s : set)
			stack.push(s);
		

		while (stack.size()) {
			state_t s = stack.top();
			stack.pop();

			// check if the state has
			FSMStateSetType epsilonTransitions = this->m_TransitionFunc(s, '\0');

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
	inline FSMStateSetType NonDeterFiniteAutomatan<TransFuncT, InputT>::_get_final_states(const FSMStateSetType& currStateSet) const
	{
		constexpr state_t startState = FiniteStateMachine<TransFuncT, InputT>::START_STATE;

		const FSMStateSetType& finalStates = this->getFinalStates();
		FSMStateSetType finalStateSet;

		auto lambda = [&finalStateSet, &finalStates](state_t s) {
			if (finalStates.contains(s))
				finalStateSet.insert(s);
		};

		std::for_each(currStateSet.begin(), currStateSet.end(), lambda);

		return finalStateSet;
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

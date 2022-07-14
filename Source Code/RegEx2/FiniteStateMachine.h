#pragma once

#include <set>
#include <vector>
#include <array>


namespace m0st4fa {


	typedef unsigned state_t;
	typedef unsigned flag_t;
	template <typename T>
	struct TransitionFunction {
		size_t m_StateMax = 0;
		size_t m_InputMax = 0;
		T m_Function;
		
		TransitionFunction() = default;
		TransitionFunction(const T& function) : m_Function(function), m_StateMax{ m_Function.size() }, m_InputMax{ m_Function.at(0).size() } {}
		
		template <typename input_t>
		state_t operator()(state_t state, input_t input) const {
			return m_Function.at(state).at(input);
		}

	};

	template<typename input_t>
	using TransFn = TransitionFunction<input_t>;

	template <typename TransFuncT, typename input_t = std::string>
	class FiniteStateMachine final {
		using set_t = std::set<state_t>;

		// private instance data members
		std::set<state_t> m_FinalStates{};
		flag_t m_Flags = nullptr;

	protected:
		TransFuncT* m_TransitionFunc = nullptr;
		static constexpr state_t m_StartState = 0;

	public:
		FiniteStateMachine() = default;
		FiniteStateMachine(TransFuncT& tranFn, const set_t& fStates, flag_t flags) :
			m_FinalStates { fStates }, m_TransitionFunc{ &tranFn }, m_Flags{ flags }
			{};

			//inline std::pair<bool, std::pair<state_t, index_t>>
			//	simulate_LP(const inputT* inputStr) {
			//	std::vector<state_t> currStates{ this->m_StartState };
			//	currStates.reserve(10);
			//	index_t currChar = 0;

			//	// while we aren't at the end of the string
			//	// or have reached the dead state
			//	while ((inputStr[currChar]) && (currStates.back() != this->m_DeadState))
			//		// push the next state into the curr states vector and move to the next char
			//		currStates.push_back(this->m_TransitionFunc[currStates.back()][*(inputStr + currChar++)]);


			//	// search for the latest state we've collected that is a final state
			//	// note that if we reach the beginning of the vector (the initial state)
			//	// it means that no final states were reached
			//	for (size_t i = currStates.size() - 1; i > 0; i--)
			//		if (includes<state_t>(this->m_FinalStates, currStates.at(i)))
			//			return std::pair<bool, std::pair<state_t, index_t>> {true, { currStates.at(i), currChar }};
			//		else
			//			currChar--;

			//	// if there are no final states in the currStates vector
			//	return std::pair<bool, std::pair<state_t, index_t>> {false, { this->m_StartState, -1 }};
			//};


	};
};
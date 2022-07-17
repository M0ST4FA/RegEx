#pragma once

#include <set>
#include <vector>
#include <array>
#include <string>
#include <iostream>


namespace m0st4fa {


	typedef unsigned state_t;
	typedef unsigned flag_t;


	template <typename TableT>
	struct TransitionFunction {
		size_t m_StateMax = 0;
		size_t m_InputMax = 0;
		TableT m_Function;
		
		TransitionFunction() = default;
		TransitionFunction(const TableT& function) : m_Function(function), m_StateMax{ m_Function.size() }, m_InputMax{ m_Function.at(0).size() } {}
		
		template <typename InputT>
		state_t operator()(state_t state, InputT input) const {
			return m_Function.at(state).at(input);
		}

	};

	template<typename TableT>
	using TransFn = TransitionFunction<TableT>;

	struct FSMResult {
		bool accepted = false;
		struct {
			unsigned long start = 0;
			unsigned long end = 0;
		} indecies;
		const std::string& input;
	};
	std::ostream& operator<<(const std::ostream&, const FSMResult&);

	enum class FSM_MODE {
		MM_WHOLE_STRING,
		MM_LONGEST_PREFIX,
		MM_LONGEST_SUBSTRING,
		MM_FSM_MODE_MAX,
	};

	template <typename TransFuncT, typename InputT = std::string>
	class FiniteStateMachine {
		using set_t = std::set<state_t>;

		// private instance data members
		std::set<state_t> m_FinalStates{};
		flag_t m_Flags;

	protected:
		TransFuncT m_TransitionFunc;
		static constexpr state_t START_STATE = 1;

	public:
		FiniteStateMachine() = default;
		FiniteStateMachine(const set_t& fStates, const TransFuncT& tranFn, flag_t flags) :
			m_FinalStates { fStates }, m_TransitionFunc{ tranFn }, m_Flags{ flags }
			{};

		const std::set<state_t>& getFinalStates() const { return m_FinalStates; };
		flag_t getFlags() const { return m_Flags; };
	};
};
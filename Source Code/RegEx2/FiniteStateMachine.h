#pragma once

#include <set>
#include <vector>
#include <array>
#include <string>
#include <iostream>


namespace m0st4fa {

	// TYPE ALIASES
	typedef unsigned state_t;
	using state_set_t = std::set<state_t>;
	typedef unsigned flag_t;

	// ENUMS
	enum class FSM_MODE {
		MM_WHOLE_STRING = 0,
		MM_LONGEST_PREFIX,
		MM_LONGEST_SUBSTRING,
		MM_FSM_MODE_MAX,
	};

	enum class FSM_TYPE {
		MT_EPSILON_NFA = 0,
		MT_NON_EPSILON_NFA,
		MT_DFA,
		MT_MACHINE_TYPE_MAX,
	};

	enum FSM_FLAG {
		//FF_CASE_INSENSITIVE,
		//FF_CASE_SENSITIVE,
		FF_FLAG_MAX
	};

	template <typename TableT>
	struct TransitionFunction {
		size_t m_StateMax = 0;
		size_t m_InputMax = 0;
		TableT m_Function;
		
		TransitionFunction() = default;
		TransitionFunction(const TableT& function) : m_Function(function), m_StateMax{ m_Function.size() }, m_InputMax{ m_Function.at(0).size() } {}
		
		template <typename InputT>
		auto operator()(state_t state, InputT input) const {
			return m_Function.at(state).at(input);
		}

	private:
		

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

	template <typename TransFuncT, typename InputT = std::string>
	class FiniteStateMachine {
		using state_set_t = std::set<state_t>;

		// private instance data members
		const std::set<state_t> m_FinalStates{};
		FSM_TYPE m_MachineType;
		flag_t m_Flags;

	protected:
		TransFuncT m_TransitionFunc;
		static constexpr state_t START_STATE = 1;

	public:
		FiniteStateMachine() = default;
		FiniteStateMachine(const state_set_t& fStates, const TransFuncT& tranFn, FSM_TYPE machineType ,flag_t flags) :
			m_FinalStates { fStates }, m_TransitionFunc{ tranFn }, m_MachineType {machineType}, m_Flags{flags}
			{};

		const std::set<state_t>& getFinalStates() const { return m_FinalStates; };
		flag_t getFlags() const { return m_Flags; };
		FSM_TYPE getMachineType() const { return m_MachineType; };
	};
};
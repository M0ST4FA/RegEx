#pragma once

#include <set>
#include <unordered_set>
#include <vector>
#include <array>
#include <string>
#include <iostream>
#include <source_location>
#include <functional>

#include "Logger.h"
#include <concepts>

namespace m0st4fa {

	// TYPE ALIASES
	typedef unsigned state_t;
	using state_set_t = std::unordered_set<state_t>;
	
	template <typename T>
	concept set_type = std::is_same_v<T, state_set_t> || std::is_same_v<T, std::vector<state_t>>;
	
	template <set_type T>
	std::ostream& operator<<(std::ostream& os, const T& set)
	{

		// if the set is empty
		if (set.empty()) {
			std::cout << "{ }";

			return os;
		}


		// if it is not empty
		std::string temp = "{ ";
		temp += std::to_string(*set.begin());
		for (auto s : set) {
			if (s == *set.begin())
				continue;

			temp += (", " + std::to_string(s));
		}
		temp += " }\n";

		std::cout << temp;

		return os;
	}

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
	

	template <size_t StateCount = 50, size_t InputCount = 'z'>
	using FSMTable = std::array<std::array<state_t, InputCount>, StateCount>;

	template <typename TableT = FSMTable<20, 'z'>>
	struct TransitionFunction {
		size_t m_StateMax = 0;
		size_t m_InputMax = 0;
		TableT m_Function;
		
		TransitionFunction() = default;
		TransitionFunction(const TableT& function) : m_Function(function), m_StateMax{ m_Function.size() }, m_InputMax{ m_Function.at(0).size() } {}

		// TODO: add this overload
		/*
			TransitionFunction(std::function<void, const TableT&> function) : m_Function(function(TableT())), m_StateMax{ m_Function.size() }, m_InputMa{ m_Function.at(0).size() } {};
		*/
		
		template <typename InputT>
		auto operator()(state_t state, InputT input) const {
			return m_Function.at(state).at(input);
		}

		template <typename InputT>
		auto operator()(state_set_t stateSet, InputT input) const {
			state_set_t res;
			
			for (state_t state : stateSet) {
				auto tmp = m_Function.at(state).at(input);
				res.insert(tmp.begin(), tmp.end());
			}
				
			return res;
		}


	};

	template<typename TableT>
	using TransFn = TransitionFunction<TableT>;


	template <typename TransFuncT, typename InputT = std::string>
	class FiniteStateMachine {

		// friends
		friend class FSMResult;
		
		// private instance data members
		const state_set_t m_FinalStates{};
		FSM_TYPE m_MachineType;
		flag_t m_Flags;

	protected:
		Logger m_Logger;
		TransFuncT m_TransitionFunc;
		
		
		// static
		static constexpr state_t START_STATE = 1;

	public:
		FiniteStateMachine() = default;
		FiniteStateMachine(const state_set_t& fStates, const TransFuncT& tranFn, FSM_TYPE machineType ,flag_t flags) :
			m_FinalStates { fStates }, m_TransitionFunc{ tranFn }, m_MachineType {machineType}, m_Flags{flags}
			{
			
			LoggerInfo loggerInfo = { 
				  .level = LOG_LEVEL::LL_ERROR, 
				  .info = { .errorType = ERROR_TYPE::ET_INVALID_ARGUMENT} 
			  };

			if (fStates.empty()) {
				const std::string message = "FiniteStateMachine: the set of final states cannot be empty.";
				m_Logger.log(loggerInfo, message);
				throw std::invalid_argument(message);
			};

			if (machineType == FSM_TYPE::MT_MACHINE_TYPE_MAX) {
				const std::string message = R"(FiniteStateMachine: the machine type is invalid.)";
				m_Logger.log(loggerInfo, message);
				throw std::invalid_argument(message);
			};
		
		};

		const state_set_t& getFinalStates() const { return m_FinalStates; };
		flag_t getFlags() const { return m_Flags; };
		FSM_TYPE getMachineType() const { return m_MachineType; };
	};

	struct FSMResult {
		bool accepted = false;
		state_set_t finalState = { FiniteStateMachine<state_t>::START_STATE };
		struct {
			unsigned long start = 0;
			unsigned long end = 0;
		} indecies;
		const std::string& input;

	};
	std::ostream& operator<<(const std::ostream&, const FSMResult&);
};
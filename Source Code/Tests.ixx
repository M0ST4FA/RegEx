module;

#include <string>
#include <iostream>
#include <map>

#include "FiniteStateMachine.h"
#include "Parser.h"

export module Tests;

using m0st4fa::state_t;
using m0st4fa::state_set_t;
using m0st4fa::Symbol;
export typedef std::array<std::array<state_t, 'z'>, 10> table_t;

export template<typename T>
constexpr void initTranFn_ab(T& fun) {
	fun[1]['a'] = 2;
	fun[2]['a'] = 3;
	fun[3]['a'] = 3;
	fun[3]['b'] = 4;
	fun[4]['a'] = 3;
	fun[4]['b'] = 5;
	fun[5]['a'] = 5;
	fun[5]['b'] = 5;
}

export template<typename T>
constexpr void initTranFn_ab_NFA(T& fun) {

	/*
	  -> a -> a
	s    |
	  -> b
	*/

	fun[1]['a'] = { 2 };
	fun[1]['b'] = { 3 };

	fun[2]['a'] = { 2, 4 };
	fun[2]['b'] = { 3 };

	fun[3]['a'] = { 2, 4 };
	fun[3]['b'] = { 3 };

	fun[4]['a'] = { 2, 4 };
	fun[4]['b'] = { 2 };

};

export enum TOKEN {
	T_AAB,
	T_EOF,
	T_MAX,
};

export struct token_t {
	TOKEN type = T_EOF;
	std::string lexeme;
};

export std::ostream& operator<<(std::ostream&, const token_t);

export token_t fact(m0st4fa::state_t, std::string);

// PARSER -----------------------------------------------------------

export enum struct _TERMINAL {
	T_ID,
	T_PLUS,
	T_STAR,
	T_LEFT_PAREN,
	T_RIGHT_PAREN,
	T_EOF,
	T_EPSILON,
	T_NUM
};

export std::ostream& operator<<(std::ostream&, const _TERMINAL);

// so that no collisions occur with map
export enum struct _NON_TERMINAL {
	NT_E = (unsigned)_TERMINAL::T_NUM,
	NT_EP,
	NT_T,
	NT_TP,
	NT_F,
	NT_NUM
};
export std::ostream& operator<<(std::ostream&, const _NON_TERMINAL);


export bool operator<(const Symbol<_TERMINAL, _NON_TERMINAL> lhs,  const Symbol<_TERMINAL, _NON_TERMINAL> other) {
	return (unsigned)lhs.as.terminal < (unsigned)other.as.terminal;
}


export std::vector<m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>> grammer_expression();
export void define_table_llparser(m0st4fa::LLParsingTable<>&);
export void initFSMTable_parser(m0st4fa::FSMTable<>&);
export m0st4fa::Token<_TERMINAL> token_fact_parser(m0st4fa::state_t, std::string);
	
module;

#include <string>
#include <iostream>
#include <map>

#include "FiniteStateMachine.h"
#include "Parser.h"

export module Tests;

using m0st4fa::state_t;
using m0st4fa::state_set_t;
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

// PARSER

export enum struct _TERMINAL {
	T_ID,
	T_PLUS,
	T_STAR,
	T_LEFT_PAREN,
	T_RIGHT_PAREN,
	T_NUM
};

export struct _Token {
	_TERMINAL name;
	std::string lexeme;
};

// so that no collisions occur with map
export enum struct _NON_TERMINAL {
	NT_E = (unsigned)_TERMINAL::T_NUM,
	NT_EP,
	NT_T,
	NT_TP,
	NT_F,
	NT_NUM
};

export struct _Symbol {
	bool isTerminal = true;
	
	union {
		_TERMINAL terminal;
		_NON_TERMINAL nonTerminal;
	} as;

	bool operator== (const _Token& other) const {

		if (isTerminal) {
			return as.terminal == other.name;
		}
		
		return false;
	}
	
};

// for map
export bool operator<(const _Symbol lhs,  const _Symbol other) {
	return (unsigned)lhs.as.terminal < (unsigned)other.as.terminal;
}


export std::vector<m0st4fa::ProductionRecord<_Symbol>> grammer_expression();
export void define_table_llparser(m0st4fa::LLParsingTable<50, 'z'>&);
export void initFSMTable_parser(m0st4fa::FSMTable<50, 'z'>&);
export _Token token_fact_parser(m0st4fa::state_t, std::string);
	
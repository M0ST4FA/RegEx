module;

#include <string>
#include <iostream>
#include "FiniteStateMachine.h"

export module Tests;

using m0st4fa::state_t;
using m0st4fa::state_set_t;

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
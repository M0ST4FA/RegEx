module;

#include <string>
#include <iostream>
#include "FiniteStateMachine.h"

module Tests;

token_t fact(m0st4fa::state_t state, std::string lexeme) {

	switch (state) {
	case 5:
		return { T_AAB, lexeme };

	default:
		fprintf(stderr, "Error: unexpected state %d\n", state);
		throw std::runtime_error("Unreachable");
	}

	return token_t{};
};

std::ostream& operator<<(std::ostream& os, const token_t token) {
	printf("Lexeme: %s\n", token.lexeme.c_str());
	return os;
};
#include "regex.h"
#include "NFA.h"

using namespace m0st4fa;

#include <iostream>
#include <array>


template<typename T>
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

template<typename T>
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


#define TEST_LA


#ifdef TEST_REGEX

int main(void) {
	
	regex re; // = RegularExpression {pattern, options}; // the regular expression is compiled by the constructor.
	regexRes reres; // re.exec(string); // you execute the compiled regular expression against the string
	bool match; // re.match(string);    // asserts that the string matches the regular expression
	// re.getSource() => gets the source code of the regex that has been compiled
	// re.getFlags() => returns the flags applied to the regex
	// re.getIndex() => returns the index into the string of the next character that will be matched by the regex
	

	return 0;
}

#elif defined TEST_PARSER
	
int main(void) {
	typedef struct table_t {};
	typedef struct symbol_t {};

	LRParser<table_t, symbol_t> parser; // {lexicalAnalyzer, parsingTable}
	//auto parserRes = parser.parse();    // (action, executionOrder)


	return 0;
}

#elif defined TEST_LA

enum TOKEN {
	T_AAB,
	T_MAX,
};

struct token_t {
	TOKEN type;
	std::string lexeme;
	
};

token_t fact(state_t state, std::string lexeme) {
	
	switch (state) {
	case 5:
		return { T_AAB, lexeme };
	
	default:
		fprintf(stderr, "Error: unexpected state %d\n", state);
		throw std::runtime_error("Unreachable");
	}

	return token_t{};
};

int main(void) {
	typedef std::array<std::array<state_t, 'z'>, 10> table_t;


	table_t input{};
	initTranFn_ab(input);

	TransitionFunction<table_t> tf{ input };

	DFA<TransitionFunction<table_t>> automaton{ state_set_t{5}, tf };

	std::string src = "aaabbaffbababa";
	LexicalAnalyzer<token_t, table_t> lexicalAnal{ automaton, fact, src };

	token_t token = lexicalAnal.getNextToken();
	std::pair<size_t, size_t> pos = lexicalAnal.getPosition();
	printf("(%u, %u) ", (unsigned)pos.first, (unsigned)pos.second);
	printf("Lexeme: %s\n", token.lexeme.c_str());
	

	std::string x;

	while (x != "q") {
		std::cin >> x;
		if (x == "q") break;
	}
	
}

#elif defined TEST_DFA



int main(void) {
	typedef std::array<std::array<state_set_t, 'z'>, 10> table_t;

	table_t input{};
	initTranFn_ab_NFA(input);

	TransitionFunction<table_t> tf{ input };


	// TODO: next state is a set now
	/*state_t nextState = tf(1, 'a');
	std::cout << nextState << std::endl;*/

	state_set_t fstates = {4, 2};
	NFA<TransFn<table_t>, std::string> automaton{ fstates, tf, FSM_TYPE::MT_NON_EPSILON_NFA };

	std::string str = "aaababffaba";
	auto result = automaton.simulate(str, FSM_MODE::MM_LONGEST_SUBSTRING);
	std::cout << result << "\n";

	std::string x;
	
	while (x != "q") {
		std::cin >> x;
		if (x == "q") break;
	}
	
	
}

#endif


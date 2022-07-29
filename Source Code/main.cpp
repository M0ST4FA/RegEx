#include "regex.h"
#include "NFA.h"

using namespace m0st4fa;

#include <iostream>
#include <array>

#define TEST_DFA


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

int main(void) {
	typedef struct input_t {};
	typedef struct token_t {};

	LexicalAnalyzer<token_t, input_t> la; // {input, stateMachine}
	
}

#elif defined TEST_DFA

// TODO: create a version for NFA
template<typename T>
constexpr void initTranFn_ab(T& fun) {
	fun[1]['a']= 2;
	fun[2]['a'] = 3;
	fun[3]['a'] = 1+2;
	fun[3]['b'] = 2+2;
	fun[4]['a'] = 1+2;
	fun[4]['b'] = 3+2;
	fun[5]['a'] = 3+2;
	fun[5]['b'] = 3+2;
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


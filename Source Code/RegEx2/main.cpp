#include "regex.h"


using namespace m0st4fa;

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
int main(void) {
	typedef struct input_t {};
	typedef struct transfn_t {};

	TransitionFunction<input_t> tf;
	// state_t nextState = tf[cstate][cinput];

	DFA<transfn_t, input_t> automaton; // {tranfn}

}


#endif


// std includes
#include <iostream>
#include <array>

// my includes
#include "regex.h"
#include "DFA.h"
#include "LLParser.hpp"
import Tests;

// using directives
using namespace m0st4fa;

// #defines


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
	

	table_t fsmTable{};
	initFSMTable_parser(fsmTable);

	TransitionFunction<table_t> tf_parser{ fsmTable };

	DFA<TransitionFunction<table_t>> automaton_parser{ state_set_t{3, 4, 5, 6, 7}, tf_parser };

	std::string src = "id + id * id";
	LexicalAnalyzer<_Token, table_t> lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

	LLParsingTable table{};
	define_table_llparser(table);

	LLParser <_Symbol, _Token > parser{ grammer_expression(), lexicalAnal_parser, table, _Symbol{false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	
	parser.parse(m0st4fa::ExecutionOrder::EO_INORDER);

	std::string x;
	while (x != "q") {
		std::cin >> x;
		if (x == "q") break;
	}
	
	return 0;
}

#elif defined TEST_LA


int main(void) {
	typedef std::array<std::array<state_t, 'z'>, 10> table_t;


	table_t input{};
	initTranFn_ab(input);

	TransitionFunction<table_t> tf{ input };

	DFA<TransitionFunction<table_t>> automaton{ state_set_t{5}, tf };

	std::string src = "aaabb\taaabb";
	LexicalAnalyzer<token_t, table_t> lexicalAnal{ automaton, fact, src };

	// get next token
	token_t token = lexicalAnal.getNextToken();

	while (token.type != T_EOF) {
		std::pair<size_t, size_t> pos = lexicalAnal.getPosition();
		printf("(%u, %u) ", (unsigned)pos.first, (unsigned)pos.second);
		std::cout << token;

		token = lexicalAnal.getNextToken((unsigned)LA_FLAG::LAF_ALLOW_WHITE_SPACE_CHARS);
	}

	// infinite loop
	std::string x;

	while (x != "q") {
		std::cin >> x;
		if (x == "q") break;
	}
	
}

#elif defined TEST_FSM



int main(void) {
	typedef std::array<std::array<state_set_t, 'z'>, 10> table_t;

	table_t input{};
	initTranFn_ab_NFA(input);

	TransitionFunction<table_t> tf{ input };


	// TODO: next state is a set now
	/*state_t nextState = tf(1, 'a');
	std::cout << nextState << std::endl;*/

	state_set_t fstates = {4, 2};
	NFA<TransFn<table_t>, std::string> automaton{ fstates, tf, FSM_TYPE::MT_EPSILON_NFA };

	std::string str = "aaababffaba";
	auto result = automaton.simulate(str, FSM_MODE::MM_LONGEST_SUBSTRING);
	std::cout << result << "\n";

	std::string x;
	

	typedef std::array<std::array<state_t, 'z'>, 10> table_t_dfa;


	table_t_dfa input_dfa{};
	initTranFn_ab(input_dfa);

	TransitionFunction<table_t_dfa> tfDFA{ input_dfa };
	state_set_t fstatesDFA = state_set_t{ 4 };

	DFA<TransitionFunction<table_t_dfa>> automatonDFA{ fstatesDFA, tfDFA };

	auto resultDFA = automatonDFA.simulate(str, FSM_MODE::MM_LONGEST_SUBSTRING);
	std::cout << resultDFA << "\n";
	
	while (x != "q") {
		std::cin >> x;
		if (x == "q") break;
	}
}

#endif


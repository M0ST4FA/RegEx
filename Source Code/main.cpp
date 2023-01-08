// std includes
#include <iostream>
#include <array>

// my includes
#include "regex.h"
#include "DFA.h"
#include "LLParser.hpp"
#include "LRParser.hpp"
#include "LLPGenerator.h"
#include "LRPGenerator.h"
#include "LexicalAnalyzer.h"
#include "ANSI.h"

import Tests;

// using declaraEtions
using m0st4fa::TransitionFunction;
using m0st4fa::DFA;
using m0st4fa::FSMTable;
using m0st4fa::state_set_t;
using m0st4fa::state_t;
using m0st4fa::LexicalAnalyzer;
using m0st4fa::Token;
using LexicalAnalyzerType = LexicalAnalyzer<Token<_TERMINAL>>;

using ItemType = m0st4fa::Item<LRProductionType>;
using ItemSet = m0st4fa::ItemSet<ItemType>;

using m0st4fa::LLActionRecord;
using m0st4fa::LLSynthesizedRecord;

using GrammarType = m0st4fa::ProductionVector<ProductionType>;
using FirstType = std::vector<std::set<Symbol>>;

using LLParserType = m0st4fa::LLParser<GrammarType, LexicalAnalyzerType, Symbol>;
using LLParsingTableType = m0st4fa::LLParsingTable<GrammarType>;
using LLParserGeneratorType = m0st4fa::LLParserGenerator<GrammarType, LLParsingTableType>;

using LRParsingTableType = m0st4fa::LRParsingTable<LRGrammarType>;
using LRParserType = m0st4fa::LRParser<LRGrammarType, LexicalAnalyzerType, Symbol, LRStateType, LRParsingTableType>;
using LRStackType = m0st4fa::LRStackType<LRDataType, TokenType>;
using m0st4fa::LRState;
using LRParserGeneratorType = m0st4fa::LRParserGenerator<LRGrammarType, ItemType, LRParsingTableType>;

using m0st4fa::regex::regex;
using m0st4fa::regex::regexRes;

// #defines
#define TEST_REGEX

#ifdef TEST_REGEX

int main(void) {

	// NEXT TO WORK ON: FOLLOWPOS IS CALCULATED INCORRECTLY


	regex re{ R"(a(b|c)\x)", "g" }; // = RegularExpression {pattern, options}; // the regular expression is compiled by the constructor.
	regexRes reres = re.exec("mostafa"); // you execute the compiled regular expression against the string
	bool match = re.match("mostafa");    // asserts that the string matches the regular expression
	re.getPattern();
	re.getFlags(); 
	re.getIndex();

	return 0;
}
#elif defined TEST_LR_PARSER

int main(int argc, char** argv) {
	FSMTable<> fsmTable{};
	initFSMTable_parser(fsmTable);

	TransitionFunction<FSMTable<>> tf_parser{ fsmTable };

	DFA<TransitionFunction<FSMTable<>>> automaton_parser{ state_set_t{3, 4, 5, 6, 7}, tf_parser };

	// if no arguments are passed
	if (argc < 2)
		while (true) {
			std::string src;
			std::cout << ANSI_ESC"[36m""Enter the source code to be parsed : " ANSI_ESC"[0m";
			std::getline(std::cin, src);
			std::cout << "\n";

			if (src == "q" || src == "Q")
				return 0;

			LexicalAnalyzerType lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

			auto startSym = Symbol{ false, {.nonTerminal = _NON_TERMINAL::NT_E} };

			// LR GRAMMAR
			auto grammarLR = grammar_expression_LR();

			// ITEM
			const ItemType item{ grammarLR.at(0), 1,
				{toSymbol(_TERMINAL::T_EPSILON), toSymbol(_TERMINAL::T_EOF), toSymbol(_TERMINAL::T_ID) } };
			const ItemType item2{ grammarLR.at(1), 1,
				{toSymbol(_TERMINAL::T_EPSILON), toSymbol(_TERMINAL::T_EOF)} };
			ItemSet itemSet{ item, item2 };
			itemSet.insert({ grammarLR.at(1), 0, {toSymbol(_TERMINAL::T_ID)} });

			LRParsingTableType LRParsingTable;
			define_table_lrparser(LRParsingTable);

			LRParserGeneratorType parserGenerator{ grammarLR, startSym };

			try {
				//LRParsingTableType SLRParsingTable = parserGenerator.generateSLRParser();
				// CLR PARSER GENERATOR FINISHED, BUT TRY TO FACTOR OUT CERTAIN COMMON FUNCTIONALITY BETWEEN THE DIFFERENT PARSER GENERATORS.
				LRParsingTableType LALRParsingTable = parserGenerator.generateLALRParser();

				LRParserType parser{ lexicalAnal_parser, LALRParsingTable, startSym };
				parser.parse(m0st4fa::ErrorRecoveryType::ERT_PANIC_MODE);

				// ITEM TESTS
				std::cout << "\nITEM SET:\n" << (std::string)itemSet << "\n";
				std::cout << "Does the `item1` equal `item2`? " << std::boolalpha << (item == item2) << "\n";
				std::cout << "Does `itemSet` contain `item2`? " << (itemSet.contains(item2)) << "\n";
			}
			catch (std::exception& e) {
				std::cout << "Exception : " << e.what() << "\n";
			};
		};

	{
		// get the source
		std::string src = argv[1];
		std::cout << src << std::endl;

		LexicalAnalyzerType lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

		auto startSym = Symbol{ false, {.nonTerminal = _NON_TERMINAL::NT_E} };

		// LR GRAMMAR
		auto grammarLR = grammar_expression_LR();

		// ITEM
		const ItemType item{ grammarLR.at(0), 1,
			{toSymbol(_TERMINAL::T_EPSILON), toSymbol(_TERMINAL::T_EOF), toSymbol(_TERMINAL::T_ID) } };
		const ItemType item2{ grammarLR.at(1), 1,
			{toSymbol(_TERMINAL::T_EPSILON), toSymbol(_TERMINAL::T_EOF)} };
		ItemSet itemSet{ item, item2 };
		itemSet.insert({ grammarLR.at(1), 0, {toSymbol(_TERMINAL::T_ID)} });

		LRParsingTableType LRParsingTable;
		define_table_lrparser(LRParsingTable);

		LRParserGeneratorType parserGenerator{ grammarLR, startSym };

		try {
			LRParsingTableType SLRParsingTable = parserGenerator.generateSLRParser();

			LRParserType parser{ lexicalAnal_parser, SLRParsingTable, startSym };
			parser.parse(m0st4fa::ErrorRecoveryType::ERT_PANIC_MODE);

			// ITEM TESTS
			std::cout << "\nITEM SET:\n" << (std::string)itemSet << "\n";
			std::cout << "Does the `item1` equal `item2`? " << std::boolalpha << (item == item2) << "\n";
			std::cout << "Does `itemSet` contain `item2`? " << (itemSet.contains(item2)) << "\n";
		}
		catch (std::exception& e) {
			std::cout << "Exception : " << e.what() << "\n";
		};
	}

	return 0;
}

#elif defined TEST_LL_PARSER
int main(int argc, char** argv) {
	FSMTable<> fsmTable{};
	initFSMTable_parser(fsmTable);

	TransitionFunction<FSMTable<>> tf_parser{ fsmTable };

	DFA<TransitionFunction<FSMTable<>>> automaton_parser{ state_set_t{3, 4, 5, 6, 7}, tf_parser };

	// if no arguments are passed
	if (argc < 2)
		while (true) {
			std::string src;
			std::cout << ANSI_ESC"[36m""Enter the source code to be parsed : " ANSI_ESC"[0m";
			std::getline(std::cin, src);
			std::cout << "\n";

			if (src == "q" || src == "Q")
				return 0;

			LexicalAnalyzerType lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

			auto startSym = Symbol{ false, {.nonTerminal = _NON_TERMINAL::NT_E} };

			// LL GRAMMAR
			auto grammar = grammer_expression();
			LLParsingTableType table{ grammar };
			LLParserGeneratorType parserGenerator{ grammar, startSym };

			// LR GRAMMAR
			auto grammarLR = grammar_expression_LR();
			std::cout << grammar.at(0);

			// ITEM
			const ItemType item{ grammarLR.at(0), 0,
				{toSymbol(_TERMINAL::T_EPSILON), toSymbol(_TERMINAL::T_EOF), toSymbol(_TERMINAL::T_ID) } };
			const ItemType item2{ grammarLR.at(1), 0,
				{toSymbol(_TERMINAL::T_EPSILON), toSymbol(_TERMINAL::T_EOF)} };
			ItemSet itemSet{ item, item2 };
			itemSet.insert({ grammarLR.at(1), 0, {toSymbol(_TERMINAL::T_ID)} });

			// STATE
			LRStateType state{ 0 };

			// parse entered source
			try {
				// LL GRAMMAR TESTS
				//LLParsingTableType table = parserGenerator.generateLLParser();
				//LLParserType parser{ startSym, table, lexicalAnal_parser };
				//parser.parse(m0st4fa::ErrorRecoveryType::ERT_PANIC_MODE);
				//parserGenerator.generateLLParser();

				// LR GRAMMAR TESTS
				grammarLR.calculateFIRST();
				grammarLR.calculateFOLLOW();
				std::cout << "TRYING CREATE PARSING TABLE FOR LR GRAMMAR:\n";

				// ITEM TESTS
				std::cout << "\nITEM SET:\n" << (std::string)itemSet << "\n";
				std::cout << "Does the `item1` equal `item2`? " << std::boolalpha << (item == item2) << "\n";
				std::cout << "Does `itemSet` contain `item2`? " << (itemSet.contains(item2)) << "\n";
				itemSet.CLOSURE(&grammarLR);
				itemSet.GOTO(toSymbol(_NON_TERMINAL::NT_E), &grammarLR);
			}
			catch (std::exception& e) {
				std::cout << "Exception : " << e.what() << "\n";
			};
		};

	{
		// get the source
		std::string src = argv[1];

		LexicalAnalyzer<Token<_TERMINAL>, FSMTable<>> lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

		// create parser object
		auto startSym = Symbol{ false, {.nonTerminal = _NON_TERMINAL::NT_E} };

		// LL GRAMMAR
		auto grammar = grammer_expression();
		LLParsingTableType table{ grammar };
		LLParserGeneratorType parserGenerator{ grammar, startSym };

		LLParserType parser{ startSym, table, lexicalAnal_parser };

		// parse entered source
		try {
			parser.parse(m0st4fa::ErrorRecoveryType::ERT_PANIC_MODE);
			grammar.calculateFIRST();
			grammar.calculateFOLLOW();
		}
		catch (std::exception& e) {
			std::cout << "Exception : " << e.what() << "\n";
		};
	}

	return 0;
}

#elif defined TEST_PARSER

int main(int argc, char** argv) {
	FSMTable<> fsmTable{};
	initFSMTable_parser(fsmTable);

	TransitionFunction<FSMTable<>> tf_parser{ fsmTable };

	DFA<TransitionFunction<FSMTable<>>> automaton_parser{ state_set_t{3, 4, 5, 6, 7}, tf_parser };

	// if no arguments are passed
	if (argc < 2)
		while (true) {
			std::string src;
			std::cout << ANSI_ESC"[36m""Enter the source code to be parsed : " ANSI_ESC"[0m";
			std::getline(std::cin, src);
			std::cout << "\n";

			if (src == "q" || src == "Q")
				return 0;

			LexicalAnalyzer<Token<_TERMINAL>, FSMTable<>> lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

			LLParsingTable<> table{};
			define_table_llparser(table);

			// create parser object
			auto startSym = Symbol{ false, {.nonTerminal = _NON_TERMINAL::NT_E} };
			auto grammar = grammer_expression();
			m0st4fa::ProdVec<Symbol, Synthesized, Action> prodVec{ grammar };

			auto grammarLR = grammar_expression_LR();

			m0st4fa::LLParser <
				Symbol,
				LLSynthesizedRecord<SynData>,
				SynData,
				LLActionRecord<ActData>,
				ActData,
				Token<_TERMINAL>
			>
				parser{ grammar, startSym, table, lexicalAnal_parser };

			// parse entered source
			try {
				parser.parse(m0st4fa::ErrorRecoveryType::ERT_PANIC_MODE);
				prodVec.calculateFIRST();
				prodVec.calculateFIRST();
				prodVec.calculateFOLLOW();
				prodVec.getFIRST(_NON_TERMINAL::NT_E);
				grammarLR.calculateFIRST();
				grammarLR.calculateFOLLOW();
				grammarLR.calculateFOLLOW();
				grammarLR.getFOLLOW(_NON_TERMINAL::NT_E);
				m0st4fa::SymbolString<_TERMINAL, _NON_TERMINAL> symString{ toSymbol(_TERMINAL::T_EPSILON), toSymbol(_NON_TERMINAL::NT_E) };
				std::cout << "Grammar symbol string: " << symString << std::endl;
				symString.calculateFIRST(grammarLR.getFIRST());
			}
			catch (std::exception& e) {
				std::cout << "Exception : " << e.what() << "\n";
			};
		};

	{
		// get the source
		std::string src = argv[1];

		LexicalAnalyzer<Token<_TERMINAL>, FSMTable<>> lexicalAnal_parser{ automaton_parser, token_fact_parser, src };

		LLParsingTable<> table{};
		define_table_llparser(table);

		// create parser object
		auto startSym = Symbol{ false, {.nonTerminal = _NON_TERMINAL::NT_E} };
		auto grammar = grammer_expression();
		m0st4fa::ProdVec<Symbol, Synthesized, Action> prodVec{ grammar };

		m0st4fa::LLParser <
			Symbol,
			LLSynthesizedRecord<SynData>,
			SynData,
			LLActionRecord<ActData>,
			ActData,
			Token<_TERMINAL>
		>
			parser{ grammar, startSym, table, lexicalAnal_parser };

		// parse entered source
		try {
			parser.parse(m0st4fa::ErrorRecoveryType::ERT_PANIC_MODE);
			prodVec.calculateFIRST();
			prodVec.calculateFOLLOW();
		}
		catch (std::exception& e) {
			std::cout << "Exception : " << e.what() << "\n";
		};
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

	state_set_t fstates = { 4, 2 };
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
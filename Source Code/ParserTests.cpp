module;

#include <string>
#include <iostream>
#include <map>

#include "FiniteStateMachine.h"
#include "LexicalAnalyzer.h"
#include "Parser.h"

module Tests;

using m0st4fa::Token;

void synDataAct(Stack& stack, SynData& data) {
	
	std::cout << data.str << ", stack size: " << stack.size() + 1 << "\n\n";
}

void actDataAct(Stack& stack, ActData& data) {

	unsigned currIndex = stack.size() + 1;
	unsigned actIndex = currIndex - 3;

	static char msg[100] = "[Modified] ";
	const char* original = stack.at(actIndex).as.synRecord.data.str;

	if (strlen(msg) < 20)
		strcat(msg, original);

	stack.at(actIndex).as.synRecord.data.str = msg;

	std::cout << stack.at(actIndex).type << "\n";
	printf("[Action at index: %u] Assigned data to synthesized record at index: %u\n\n", currIndex, actIndex);
}

GrammarType grammer_expression() {
	using StackElement = m0st4fa::LLStackElement<Symbol, Synthesized, Action>;
	using Production = m0st4fa::ProductionRecord<Symbol, StackElement>;


	std::vector<Production> result;

	Production prod;
	StackElement se_E = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} } };
	StackElement se_EP = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} } };
	StackElement se_T = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} } };
	StackElement se_TP = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} } };
	StackElement se_F = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} } };

	StackElement se_ID = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_ID}} } };
	StackElement se_PLUS = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_PLUS}} } };
	StackElement se_STAR = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_STAR}} } };
	StackElement se_LP = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} } };
	StackElement se_RP = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} } };
	StackElement se_EPS = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_EPSILON}} } };
	StackElement se_EOF = { .type = StackElement::SET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_EOF}} } };


	StackElement se_Syn = { .type = StackElement::SET_SYNTH_RECORD, .as {.synRecord = m0st4fa::SynthesizedRecord<SynData> {.action = (void*)synDataAct} } };
	StackElement se_Act = { .type = StackElement::SET_ACTION_RECORD, .as {.actRecord = m0st4fa::ActionRecord<ActData> {.action
		= (void*)actDataAct}} };

	// E -> T E'
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		 {se_T, se_Act, se_EP, se_Syn} };

	result.push_back(prod);

	// E' -> + E T
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		 {se_PLUS, se_T, se_EP} };

	result.push_back(prod);

	// T -> F T'
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		 {se_F, se_TP} };

	result.push_back(prod);


	// T' -> * F T'
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		 {se_STAR, se_Act, se_F, se_Syn, se_TP} };

	result.push_back(prod);

	// F -> (E)
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_LP, se_E, se_RP} };

	result.push_back(prod);

	// F -> ID
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_ID} };

	result.push_back(prod);

	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		 {se_EPS} };

	result.push_back(prod);

	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		 {se_EPS} };

	result.push_back(prod);

	return result;
}

m0st4fa::ProductionVector<ProductionType> grammar_expression_LR()
{
	using StackElement = m0st4fa::LLStackElement< Symbol, Synthesized, Action>;
	using Production = m0st4fa::ProductionRecord<Symbol, StackElement>;

	m0st4fa::ProductionVector<Production> result;

	Production prod;
	StackElement se_E = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} } };
	StackElement se_T = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} } };
	StackElement se_F = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} } };

	StackElement se_ID = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_ID}} } };
	StackElement se_PLUS = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_PLUS}} } };
	StackElement se_STAR = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_STAR}} } };
	StackElement se_LP = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} } };
	StackElement se_RP = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} } };
	StackElement se_EPS = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_EPSILON}} } };
	StackElement se_EOF = { .type = StackElement::SET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_EOF}} } };

	StackElement se_Syn = { .type = StackElement::SET_SYNTH_RECORD, .as {.synRecord = m0st4fa::SynthesizedRecord<SynData> {.action = (void*)synDataAct} } };
	StackElement se_Act = { .type = StackElement::SET_ACTION_RECORD, .as {.actRecord = m0st4fa::ActionRecord<ActData> {.action
		= (void*)actDataAct}} };

	// E -> E + T
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		 {se_E, se_PLUS, se_T} };

	result.pushProduction(prod);

	// E -> T

	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		 {se_T} };

	result.pushProduction(prod);

	// T -> T * F
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		 {se_T, se_Act, se_STAR, se_Syn, se_F} };

	result.pushProduction(prod);

	// T -> F
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		 {se_F} };

	result.pushProduction(prod);

	// F -> (E)
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_LP, se_E, se_RP} };

	result.pushProduction(prod);

	// F -> ID
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_ID} };

	result.pushProduction(prod);


	return result;
}

void define_table_llparser(m0st4fa::LLParsingTable<GrammarType>& table)
{
	using StackElement = m0st4fa::LLStackElement<Symbol, Synthesized, Action>;
	using Production = m0st4fa::ProductionRecord<Symbol, StackElement>;

	GrammarType grammer = grammer_expression();

	Production prod;
	StackElement se_E = { StackElement::SET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	StackElement se_EP = { StackElement::SET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} };
	StackElement se_T = { StackElement::SET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} };
	StackElement se_TP = { StackElement::SET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} };
	StackElement se_F = { StackElement::SET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} };

	StackElement se_ID = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_ID}} };
	StackElement se_PLUS = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_PLUS}} };
	StackElement se_STAR = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_STAR}} };
	StackElement se_LP = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} };
	StackElement se_RP = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} };
	StackElement se_EPS = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_EPSILON}} };
	StackElement se_EOF = { StackElement::SET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_EOF}} };

	auto token_id = Token{ _TERMINAL::T_ID, "id" };

	table[EXTRACT_VARIABLE(se_E)][EXTRACT_TERMINAL(se_ID)] = {false, 0};
	table[EXTRACT_VARIABLE(se_E)][EXTRACT_TERMINAL(se_LP)] = { false, 0 };
	
	table[EXTRACT_VARIABLE(se_EP)][EXTRACT_TERMINAL(se_PLUS)] = { false, 1 };

	table[EXTRACT_VARIABLE(se_EP)][EXTRACT_TERMINAL(se_RP)] = { false, 6 };
	table[EXTRACT_VARIABLE(se_EP)][EXTRACT_TERMINAL(se_EOF)] = { false, 6 };

	table[EXTRACT_VARIABLE(se_T)][EXTRACT_TERMINAL(se_ID)] = { false, 2 };
	table[EXTRACT_VARIABLE(se_T)][EXTRACT_TERMINAL(se_LP)] = { false, 2 };

	table[EXTRACT_VARIABLE(se_TP)][EXTRACT_TERMINAL(se_STAR)] = { false, 3 };

	table[EXTRACT_VARIABLE(se_TP)][EXTRACT_TERMINAL(se_PLUS)] = { false, 7 };
	table[EXTRACT_VARIABLE(se_TP)][EXTRACT_TERMINAL(se_RP)] = { false, 7 };
	table[EXTRACT_VARIABLE(se_TP)][EXTRACT_TERMINAL(se_EOF)] = { false, 7 };
	
	table[EXTRACT_VARIABLE(se_F)][EXTRACT_TERMINAL(se_ID)] = { false, 5 };
	table[EXTRACT_VARIABLE(se_F)][EXTRACT_TERMINAL(se_LP)] = { false, 4 };

}

void initFSMTable_parser(m0st4fa::FSMTable<>& table)
{

	// id
	table[1]['i'] = 2;
	table[2]['d'] = 3; // 3 is a final state
	
	// (
	table[1]['('] = 4; // 4 is a final state
	
	// )
	table[1][')'] = 5; // 5 is a final state
	
	// +
	table[1]['+'] = 6; // 6 is a final state
	
	// *
	table[1]['*'] = 7; // 7 is a final state
	
}

Token<_TERMINAL> token_fact_parser(m0st4fa::state_t state, std::string lexeme) {

	switch (state) {
	case 3:
		return Token{_TERMINAL::T_ID, lexeme};
		break;
	case 4:
		return Token{_TERMINAL::T_LEFT_PAREN, lexeme};
		break;
	case 5:
		return Token{_TERMINAL::T_RIGHT_PAREN, lexeme};
		break;
	case 6:
		return Token{ _TERMINAL::T_PLUS, lexeme };
		break;
	case 7:
		return Token{ _TERMINAL::T_STAR, lexeme };
		break;
	default:
		std::cerr << "Unknown state: " << state << std::endl;
		throw std::runtime_error("Unknown state");
	};
	
};

std::string stringfy(const _TERMINAL terminal) {

	_ASSERT_EXPR(terminal >= _TERMINAL::T_ID && terminal < _TERMINAL::T_COUNT, "There is no such terminal");

	static const std::map<_TERMINAL, std::string> terminal_to_string = {
		{ _TERMINAL::T_ID, "ID" },
		{ _TERMINAL::T_LEFT_PAREN, "LEFT_PAREN" },
		{ _TERMINAL::T_RIGHT_PAREN, "RIGHT_PAREN" },
		{ _TERMINAL::T_PLUS, "PLUS" },
		{ _TERMINAL::T_STAR, "STAR" },
		{ _TERMINAL::T_EOF, "EOF" },
		{ _TERMINAL::T_EPSILON, "EPSILON" }
	};

	return terminal_to_string.at(terminal);
}

std::ostream& operator<<(std::ostream& os, const _TERMINAL terminal)
{
	
	return os << stringfy(terminal);
}

std::string stringfy(const _NON_TERMINAL variable) {
	
	_ASSERT_EXPR(variable >= _NON_TERMINAL::NT_E && variable < _NON_TERMINAL::NT_COUNT, "There is no such non-terminal");

	static const std::map<_NON_TERMINAL, std::string> variable_to_string = {
		{ _NON_TERMINAL::NT_E, "E" },
		{ _NON_TERMINAL::NT_EP, "E'" },
		{ _NON_TERMINAL::NT_T, "T" },
		{ _NON_TERMINAL::NT_TP, "T'" },
		{ _NON_TERMINAL::NT_F, "F" }
	};

	return variable_to_string.at(variable);
}

std::ostream& operator<<(std::ostream& os, const _NON_TERMINAL variable)
{
	return os << stringfy(variable);
}

Symbol toSymbol(_TERMINAL terminal) {
	return Symbol{.isTerminal = true, .as = { .terminal = terminal }};
};

Symbol toSymbol(_NON_TERMINAL nonTerminal) {
	return Symbol{ .isTerminal = false, .as = {.nonTerminal = nonTerminal } };
};
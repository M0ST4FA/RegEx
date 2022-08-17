module;

#include <string>
#include <iostream>
#include <map>

#include "FiniteStateMachine.h"
#include "LexicalAnalyzer.h"
#include "Parser.h"

module Tests;

using m0st4fa::Token;
using m0st4fa::Symbol;

std::vector<m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>> grammer_expression() {
	std::vector<m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>> result;

	m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>> prod;
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_E = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_EP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_T = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_T}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_TP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_F = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_F}} };

	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_ID = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_ID}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_PLUS = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_PLUS}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_STAR = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_STAR}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_LP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_RP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_EPS = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_EPSILON}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_EOF = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_EOF}} };



	// E -> T E'
	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		.prodBody = {se_T, se_EP} };

	result.push_back(prod);

	// E' -> + E T
	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		.prodBody = {se_PLUS, se_T, se_EP} };

	result.push_back(prod);

	// T -> F T'
	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		.prodBody = {se_F, se_TP} };

	result.push_back(prod);


	// T' -> * F T'
	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		.prodBody = {se_STAR, se_F, se_TP} };

	result.push_back(prod);

	// F -> (E)
	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		.prodBody = {se_LP, se_E, se_RP} };

	result.push_back(prod);

	// F -> ID
	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		.prodBody = {se_ID} };

	result.push_back(prod);

	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		.prodBody = {se_EPS} };

	result.push_back(prod);

	prod = m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		.prodBody = {se_EPS} };

	result.push_back(prod);

	return result;
}

void define_table_llparser(m0st4fa::LLParsingTable<>& table)
{
	std::vector<m0st4fa::ProductionRecord<Symbol<_TERMINAL, _NON_TERMINAL>>> grammer = grammer_expression();

	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_E = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_EP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_T = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_T}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_TP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_F = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {false, {.nonTerminal = _NON_TERMINAL::NT_F}} };

	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_ID = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_ID}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_PLUS = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_PLUS}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_STAR = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_STAR}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_LP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_RP = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_EPS = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_EPSILON}} };
	m0st4fa::StackElement<Symbol<_TERMINAL, _NON_TERMINAL>> se_EOF = { m0st4fa::StackElementType::SET_GRAM_SYMBOL, Symbol<_TERMINAL, _NON_TERMINAL> {true, {.terminal = _TERMINAL::T_EOF}} };

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


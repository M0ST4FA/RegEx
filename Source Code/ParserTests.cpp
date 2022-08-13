module;

#include <string>
#include <iostream>
#include "FiniteStateMachine.h"
#include "Parser.h"

module Tests;

std::vector<m0st4fa::ProductionRecord<_Symbol>> grammer_expression() {
	std::vector<m0st4fa::ProductionRecord<_Symbol>> result;

	m0st4fa::ProductionRecord<_Symbol> prod;
	m0st4fa::StackElement<_Symbol> se_E = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	m0st4fa::StackElement<_Symbol> se_EP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} };
	m0st4fa::StackElement<_Symbol> se_T = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} };
	m0st4fa::StackElement<_Symbol> se_TP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} };
	m0st4fa::StackElement<_Symbol> se_F = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} };

	m0st4fa::StackElement<_Symbol> se_ID = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_ID}} };
	m0st4fa::StackElement<_Symbol> se_PLUS = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_PLUS}} };
	m0st4fa::StackElement<_Symbol> se_STAR = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_STAR}} };
	m0st4fa::StackElement<_Symbol> se_LP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} };
	m0st4fa::StackElement<_Symbol> se_RP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} };



	// E -> T E'
	prod = m0st4fa::ProductionRecord<_Symbol>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		.prodBody = {se_T, se_EP} };

	result.push_back(prod);

	// E' -> + E T
	prod = m0st4fa::ProductionRecord<_Symbol>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		.prodBody = {se_PLUS, se_T, se_EP} };

	result.push_back(prod);

	// T -> F T'
	prod = m0st4fa::ProductionRecord<_Symbol>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		.prodBody = {se_F, se_TP} };

	result.push_back(prod);


	// T' -> * F T'
	prod = m0st4fa::ProductionRecord<_Symbol>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		.prodBody = {se_STAR, se_F, se_TP} };

	result.push_back(prod);

	// F -> (E)
	prod = m0st4fa::ProductionRecord<_Symbol>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		.prodBody = {se_LP, se_E, se_RP} };

	result.push_back(prod);

	// F -> ID
	prod = m0st4fa::ProductionRecord<_Symbol>{
		.prodHead = {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		.prodBody = {se_ID} };

	result.push_back(prod);

	return result;
}

void define_table_llparser(m0st4fa::LLParsingTable<50, 'z'>& table)
{
	std::vector<m0st4fa::ProductionRecord<_Symbol>> grammer = grammer_expression();

	m0st4fa::StackElement<_Symbol> se_E = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	m0st4fa::StackElement<_Symbol> se_EP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} };
	m0st4fa::StackElement<_Symbol> se_T = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} };
	m0st4fa::StackElement<_Symbol> se_TP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} };
	m0st4fa::StackElement<_Symbol> se_F = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} };

	m0st4fa::StackElement<_Symbol> se_ID = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_ID}} };
	m0st4fa::StackElement<_Symbol> se_PLUS = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_PLUS}} };
	m0st4fa::StackElement<_Symbol> se_STAR = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_STAR}} };
	m0st4fa::StackElement<_Symbol> se_LP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} };
	m0st4fa::StackElement<_Symbol> se_RP = { m0st4fa::StackElementType::SE_GRAM_SYMBOL, _Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} };

	auto token_id = _Token{ _TERMINAL::T_ID, "id" };

	table[EXTRACT_VARIABLE(se_E)][EXTRACT_TERMINAL(se_ID)] = {false, 0};
	table[EXTRACT_VARIABLE(se_E)][EXTRACT_TERMINAL(se_LP)] = { false, 0 };
	
	table[EXTRACT_VARIABLE(se_EP)][EXTRACT_TERMINAL(se_PLUS)] = { false, 1 };

	// TODO: E' -> epsilon, _TOKEN_EOF
	table[EXTRACT_VARIABLE(se_EP)][EXTRACT_TERMINAL(se_RP)] = { false, 0 };
	table[EXTRACT_VARIABLE(se_EP)][EXTRACT_TERMINAL(se_EOF)] = { false, 0 };

	

}

void initFSMTable_parser(table_t& table)
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
;

// return the default production or the found production
m0st4fa::ProductionRecord<_Symbol> LLParsingTable::operator()(_Symbol non_terminal, _Token terminal)
{
	auto it = this->table.find(non_terminal);

	if (it == this->table.end())
		return m0st4fa::ProductionRecord<_Symbol>{};
	else
		return it->second;
	
}


_Token token_fact_parser(m0st4fa::state_t state, std::string lexeme) {

	switch (state) {
	case 3:
		return _Token{_TERMINAL::T_ID, lexeme};
		break;
	case 4:
		return _Token{_TERMINAL::T_LEFT_PAREN, lexeme};
		break;
	case 5:
		return _Token{_TERMINAL::T_RIGHT_PAREN, lexeme};
		break;
	case 6:
		return _Token{ _TERMINAL::T_PLUS, lexeme };
		break;
	case 7:
		return _Token{ _TERMINAL::T_STAR, lexeme };
		break;
	default:
		std::cerr << "Unknown state: " << state << std::endl;
		throw std::runtime_error("Unknown state");
	};
	
};

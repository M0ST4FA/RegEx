module;

#include <string>
#include <iostream>
#include <map>

#include "FiniteStateMachine.h"
#include "LexicalAnalyzer.h"
#include "Parser.h"
#include "common.h"

module Tests;

using m0st4fa::Token;
using m0st4fa::ProdElementType;

void synDataAct(LLStackType& stack, SynData& data) {
	
	std::cout << data.str << ", stack size: " << stack.size() + 1 << "\n\n";
}

void actDataAct(LLStackType& stack, ActData& data) {

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
	size_t index = 0;

	Production prod;
	StackElement se_E = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = {.gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} } };
	StackElement se_EP = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} } };
	StackElement se_T = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} } };
	StackElement se_TP = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} } };
	StackElement se_F = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} } };

	StackElement se_ID = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_ID}} } };
	StackElement se_PLUS = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_PLUS}} } };
	StackElement se_STAR = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_STAR}} } };
	StackElement se_LP = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} } };
	StackElement se_RP = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} } };
	StackElement se_EPS = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_EPSILON}} } };
	StackElement se_EOF = { .type = ProdElementType::PET_GRAM_SYMBOL, .as = { .gramSymbol = Symbol {true, {.terminal = _TERMINAL::T_EOF}} } };


	StackElement se_Syn = { .type = ProdElementType::PET_SYNTH_RECORD, .as {.synRecord = m0st4fa::LLSynthesizedRecord<SynData> {.action = (void*)synDataAct} } };
	StackElement se_Act = { .type = ProdElementType::PET_ACTION_RECORD, .as {.actRecord = m0st4fa::LLActionRecord<ActData> {.action
		= (void*)actDataAct}} };

	// E -> T E'
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		 {se_T, se_Act, se_EP, se_Syn}, index++ };

	result.push_back(prod);

	// E' -> + E T
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		 {se_PLUS, se_T, se_EP}, index++ };

	result.push_back(prod);

	// T -> F T'
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		 {se_F, se_TP}, index++ };

	result.push_back(prod);


	// T' -> * F T'
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		 {se_STAR, se_Act, se_F, se_Syn, se_TP}, index++ };

	result.push_back(prod);

	// F -> (E)
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_LP, se_E, se_RP}, index++ };

	result.push_back(prod);

	// F -> ID
	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_ID}, index++ };

	result.push_back(prod);

	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_EP} },
		 {se_EPS}, index++ };

	result.push_back(prod);

	prod = Production{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_TP} },
		 {se_EPS}, index++ };

	result.push_back(prod);

	return result;
}

void num_act(LRStackType& stack, LRStateType& newState) {
	// F -> ID
	auto state = stack.back();
	newState.hasData = true;
	newState.data = { .data = m0st4fa::toInteger(state.token.attribute) };
	std::cout << "ID value: " << newState.data.data << "\n";
}

void pass_last_act(LRStackType& stack, LRStateType& newState) {
	newState.data = stack.back().data;
	newState.hasData = true;
	std::cout << "Curr value: " << newState.data.data << "\n";
}

void pass_prelast_act(LRStackType& stack, LRStateType& newState) {
	newState.data = stack.at(stack.size() - 2).data;
	newState.hasData = true;
	std::cout << "Curr value: " << newState.data.data << "\n";
}

void add_act(LRStackType& stack, LRStateType& newState) {
	size_t a = stack.at(stack.size() - 3).data.data;
	size_t b = stack.back().data.data;
	newState.data.data = a + b;
	std::cout << std::format("Added `{}` and `{}`. Result `{}`\n", a, b, a + b);
	newState.hasData = true;
}

void mult_act(LRStackType& stack, LRStateType& newState) {
	size_t a = stack.at(stack.size() - 3).data.data;
	size_t b = stack.back().data.data;
	newState.data.data = a * b;
	std::cout << std::format("Multiplied `{}` and `{}`. Result `{}`\n", a, b, a * b);
	newState.hasData = true;
}

LRGrammarType grammar_expression_LR()
{
	m0st4fa::ProductionVector<LRProductionType> result;
	size_t index = 0;

	LRProductionType prod;
	Symbol se_EP = toSymbol(_NON_TERMINAL::NT_EP);
	Symbol se_E = toSymbol(_NON_TERMINAL::NT_E);
	Symbol se_T = toSymbol(_NON_TERMINAL::NT_T);
	Symbol se_F = toSymbol(_NON_TERMINAL::NT_F);

	Symbol se_ID = toSymbol(_TERMINAL::T_ID);
	Symbol se_PLUS = toSymbol(_TERMINAL::T_PLUS);
	Symbol se_STAR = toSymbol(_TERMINAL::T_STAR);
	Symbol se_LP = toSymbol(_TERMINAL::T_LEFT_PAREN);
	Symbol se_RP = toSymbol(_TERMINAL::T_RIGHT_PAREN);
	Symbol se_EPS = toSymbol(_TERMINAL::T_EPSILON);
	Symbol se_EOF = toSymbol(_TERMINAL::T_EOF);
	
	// E' -> E
	prod = LRProductionType{
		 {se_EP },
		 {se_E}, index++ };
	prod.postfixAction = pass_last_act;
	result.pushProduction(prod);

	// E -> E + T
	prod = LRProductionType{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		 {se_E, se_PLUS, se_T}, index++ };
	prod.postfixAction = add_act;
	result.pushProduction(prod);

	// E -> T

	prod = LRProductionType{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_E} },
		 {se_T}, index++ };
	prod.postfixAction = pass_last_act;
	result.pushProduction(prod);

	// T -> T * F
	prod = LRProductionType{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		 {se_T, se_STAR, se_F}, index++ };
	prod.postfixAction = mult_act;
	result.pushProduction(prod);

	// T -> F
	prod = LRProductionType{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_T} },
		 {se_F}, index++ };
	prod.postfixAction = pass_last_act;
	result.pushProduction(prod);

	// F -> (E)
	prod = LRProductionType{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_LP, se_E, se_RP}, index++ };
	prod.postfixAction = pass_prelast_act;
	result.pushProduction(prod);


	// F -> ID
	prod = LRProductionType{
		 {false, {.nonTerminal = _NON_TERMINAL::NT_F} },
		 {se_ID}, index++ };
	prod.postfixAction = num_act;
	result.pushProduction(prod);
	
	return result;
}

void define_table_llparser(m0st4fa::LLParsingTable<GrammarType>& table)
{
	using StackElement = m0st4fa::LLStackElement<Symbol, Synthesized, Action>;
	using Production = m0st4fa::ProductionRecord<Symbol, StackElement>;

	GrammarType grammer = grammer_expression();

	Production prod;
	StackElement se_E = { ProdElementType::PET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_E}} };
	StackElement se_EP = { ProdElementType::PET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_EP}} };
	StackElement se_T = { ProdElementType::PET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_T}} };
	StackElement se_TP = { ProdElementType::PET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_TP}} };
	StackElement se_F = { ProdElementType::PET_GRAM_SYMBOL, Symbol {false, {.nonTerminal = _NON_TERMINAL::NT_F}} };

	StackElement se_ID = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_ID}} };
	StackElement se_PLUS = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_PLUS}} };
	StackElement se_STAR = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_STAR}} };
	StackElement se_LP = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_LEFT_PAREN}} };
	StackElement se_RP = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_RIGHT_PAREN}} };
	StackElement se_EPS = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_EPSILON}} };
	StackElement se_EOF = { ProdElementType::PET_GRAM_SYMBOL, Symbol {true, {.terminal = _TERMINAL::T_EOF}} };

	auto token_id = Token{ _TERMINAL::T_ID, "id" };

	table[EXTRACT_VARIABLE(se_E)][EXTRACT_TERMINAL(se_ID)] = { false, 0 };
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

};

void define_table_lrparser(m0st4fa::LRParsingTable<LRGrammarType>& table)
{
	table.reserveRows(50);
	using StackElementType = LRStateType;
	using ProductionType = LRProductionType;
	LRGrammarType grammar = grammar_expression_LR();
	table.grammar = grammar;

	// table[0][id] = s5
	table.atAction(0, _TERMINAL::T_ID) = TE_SHIFT(5);

	// table[0][(] = s4
	table.atAction(0, _TERMINAL::T_LEFT_PAREN) = TE_SHIFT(4);

	// table[1][+] = s6
	table.atAction(1, _TERMINAL::T_PLUS) = TE_SHIFT(6);

	// table[1][$] = acc
	table.atAction(1, _TERMINAL::T_EOF) = TE_ACCEPT();

	// table[2][+] = r2
	table.atAction(2, _TERMINAL::T_PLUS) = TE_REDUCE(2 - 1);

	// table[2][*] = s7
	table.atAction(2, _TERMINAL::T_STAR) = TE_SHIFT(7);

	// table[2][)/$] = r2
	table.atAction(2, _TERMINAL::T_RIGHT_PAREN) = TE_REDUCE(2 - 1);
	table.atAction(2, _TERMINAL::T_EOF) = TE_REDUCE(2 - 1);

	// table[3][+/*/)/$] = r4
	table.atAction(3, _TERMINAL::T_PLUS) = TE_REDUCE(4 - 1);
	table.atAction(3, _TERMINAL::T_STAR) = TE_REDUCE(4 - 1);
	table.atAction(3, _TERMINAL::T_RIGHT_PAREN) = TE_REDUCE(4 - 1);
	table.atAction(3, _TERMINAL::T_EOF) = TE_REDUCE(4 - 1);

	// table[4][id] = s5
	table.atAction(4, _TERMINAL::T_ID) = TE_SHIFT(5);

	// table[4][(] = s4
	table.atAction(4, _TERMINAL::T_LEFT_PAREN) = TE_SHIFT(4);

	// table[5][+/*/)/$] = r6
	table.atAction(5, _TERMINAL::T_PLUS) = TE_REDUCE(6 - 1);
	table.atAction(5, _TERMINAL::T_STAR) = TE_REDUCE(6 - 1);
	table.atAction(5, _TERMINAL::T_RIGHT_PAREN) = TE_REDUCE(6 - 1);
	table.atAction(5, _TERMINAL::T_EOF) = TE_REDUCE(6 - 1);


	// table[6][id] = s5
	table.atAction(6, _TERMINAL::T_ID) = TE_SHIFT(5);

	// table[6][(] = s4
	table.atAction(6, _TERMINAL::T_LEFT_PAREN) = TE_SHIFT(4);


	// table[7][id] = s5
	table.atAction(7, _TERMINAL::T_ID) = TE_SHIFT(5);

	// table[7][(] = s4
	table.atAction(7, _TERMINAL::T_LEFT_PAREN) = TE_SHIFT(4);

	// table[8][+] = s6
	table.atAction(8, _TERMINAL::T_PLUS) = TE_SHIFT(6);

	// table[8][)] = s11
	table.atAction(8, _TERMINAL::T_RIGHT_PAREN) = TE_SHIFT(11);

	// table[9][+] = r1
	table.atAction(9, _TERMINAL::T_PLUS) = TE_REDUCE(1);

	// table[9][*] = s7
	table.atAction(9, _TERMINAL::T_STAR) = TE_SHIFT(7);

	// table[9][)/$] = r1
	table.atAction(9, _TERMINAL::T_RIGHT_PAREN) = TE_REDUCE(1 - 1);
	table.atAction(9, _TERMINAL::T_EOF) = TE_REDUCE(1 - 1);

	// table[10][+/*/)/$] = r3
	table.atAction(10, _TERMINAL::T_PLUS) = TE_REDUCE(3 - 1);
	table.atAction(10, _TERMINAL::T_STAR) = TE_REDUCE(3 - 1);
	table.atAction(10, _TERMINAL::T_RIGHT_PAREN) = TE_REDUCE(3 - 1);
	table.atAction(10, _TERMINAL::T_EOF) = TE_REDUCE(3 - 1);

	// table[11][+/*/)/$] = r5
	table.atAction(11, _TERMINAL::T_PLUS) = TE_REDUCE(5 - 1);
	table.atAction(11, _TERMINAL::T_STAR) = TE_REDUCE(5 - 1);
	table.atAction(11, _TERMINAL::T_RIGHT_PAREN) = TE_REDUCE(5 - 1);
	table.atAction(11, _TERMINAL::T_EOF) = TE_REDUCE(5 - 1);

	// table[0][E] = 1
	table.atGoto(0, _NON_TERMINAL::NT_E) = TE_GOTO(1);

	// table[0][T] = 2
	table.atGoto(0, _NON_TERMINAL::NT_T) = TE_GOTO(2);

	// table[0][F] = 3
	table.atGoto(0, _NON_TERMINAL::NT_F) = TE_GOTO(3);

	// table[4][E] = 8
	table.atGoto(4, _NON_TERMINAL::NT_E) = TE_GOTO(8);

	// table[4][T] = 2
	table.atGoto(4, _NON_TERMINAL::NT_T) = TE_GOTO(2);

	// table[4][F] = 3
	table.atGoto(4, _NON_TERMINAL::NT_F) = TE_GOTO(3);

	// table[6][T] = 9
	table.atGoto(6, _NON_TERMINAL::NT_T) = TE_GOTO(9);

	// table[6][F] = 3
	table.atGoto(6, _NON_TERMINAL::NT_F) = TE_GOTO(3);

	// table[7][F] = 10
	table.atGoto(7, _NON_TERMINAL::NT_F) = TE_GOTO(10);
}

void initFSMTable_parser(m0st4fa::FSMTable<>& table)
{

	//// id
	//table[1]['i'] = 2;
	//table[2]['d'] = 3; // 3 is a final state [ID]

	// int [Uses the token ID for now]
	for (size_t i = (size_t)'0'; i <= (size_t)'9'; i++) {
		table[1][i] = 3;
		table[3][i] = 3; // 3 is a final state [NUM]
	}

	// (
	table[1]['('] = 4; // 4 is a final state [LEFT_PAREN]
	
	// )
	table[1][')'] = 5; // 5 is a final state [RIGHT_PAREN]
	
	// +
	table[1]['+'] = 6; // 6 is a final state [PLUS]
	
	// *
	table[1]['*'] = 7; // 7 is a final state [STAR]
	
}

Token<_TERMINAL> token_fact_parser(m0st4fa::state_t state, std::string lexeme) {

	switch (state) {
	case 3:
		return Token{_TERMINAL::T_ID, lexeme};

	case 4:
		return Token{_TERMINAL::T_LEFT_PAREN, lexeme};

	case 5:
		return Token{_TERMINAL::T_RIGHT_PAREN, lexeme};

	case 6:
		return Token{ _TERMINAL::T_PLUS, lexeme };

	case 7:
		return Token{ _TERMINAL::T_STAR, lexeme };

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

	if (not m0st4fa::withinRange((size_t)terminal, (size_t)_TERMINAL::T_ID, (size_t)_TERMINAL::T_EPSILON, true)) {
		m0st4fa::Logger logger;
		logger.log(m0st4fa::LoggerInfo::FATAL_ERROR, "The terminal being stringfied is not within range!");
		throw std::logic_error("The terminal being stringfied is not within range!");
	}

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

	if (not m0st4fa::withinRange((size_t)variable, (size_t)_NON_TERMINAL::NT_E, (size_t)_NON_TERMINAL::NT_F, true)) {
		m0st4fa::Logger logger;
		logger.log(m0st4fa::LoggerInfo::FATAL_ERROR, "The non-terminal being stringfied is not within range!");
		throw std::logic_error("The non-terminal being stringfied is not within range!");
	}

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
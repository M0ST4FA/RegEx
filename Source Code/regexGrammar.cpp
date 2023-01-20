#pragma once

#include <map>
#include <iostream>
#include <format>
#include <string>

#include "regexTypedefs.h"
#include "regex.h"
#include "common.h"

namespace m0st4fa::regex {
	
	// DATA STRUCTURES TEMPORARILY USED WHEN CREATING A REGEX OBJECT
	size_t currPos = 0;
	std::vector<Position> allPos;
	std::map<size_t, std::set<Position>> followpos;

	void set_follow(const std::set<Position>& in, const std::set<Position>& follows) {
		std::ranges::for_each(follows.begin(), follows.end(), [&in](const auto& i) {
			std::ranges::for_each(in.begin(), in.end(), [&i](const auto& j) {
				followpos[i.position].insert(j);
				});
			});
	}

	void act_pass_last(StackType& stack, StateType& newState) {
		newState.data = stack.back().data;
	};

	void act_pass_prelast(StackType& stack, StateType& newState) {
		newState.data = stack.at(stack.size() - 2).data;
	};

	template<typename DFAStateT>
	void _fill_table(std::vector<DFAStateT>& Dstates, size_t& nextStateNum, size_t markerPosition, std::pair<FSMStateSetType, DFATableType>& table)
	{

		/** 
		* for every state `i` in `Dstates`:
		*	for every position `p` in `i`:
		*		for every position `f` in follow(`p`):
		*			let c = the character that corresponds to `f`
		*			transStates[`c`].insert(`f`)
		*/

		for (size_t currStateNum = 0; currStateNum < Dstates.size(); currStateNum++) {
			std::map<char, DFAStateT> transStates;
			std::cout << "* currStateNum: " << currStateNum << ", size of Dstates: " << Dstates.size() << "\n";
			DFAStateT& stateSet = Dstates.at(currStateNum);

			auto insertMap = [&nextStateNum](std::map<char, DFAStateT>& map, const std::set<Position>& followpos) {

				for (const Position& pos : followpos) {
					char key = pos.lexeme;

					auto it = map.find(key);
					bool exists = it != map.end();

					std::cout << std::format("	Inserting {} into key '{}'\n", (std::string)pos, key);

					if (exists)
						it->second.insert(pos);
					else
						map.insert_or_assign(key, DFAStateT{ {pos}, 0 });

				}
			};

			// populate DStates
			for (const Position& pos : stateSet) {
				const std::set<Position>& followPos = followpos[pos.position];

				if (followPos.empty())
					continue;

				std::cout << std::format("followpos of position {}: {}\n", (std::string)pos, m0st4fa::toString(followPos));

				insertMap(transStates, followPos);
			}

			// set the transition table
			for (std::pair<const char, DFAStateT>& state : transStates) {
				const auto& it = std::find(Dstates.begin(), Dstates.end(), state.second);
				bool found = it != Dstates.end();

				// if the state has not been found
				if (not found) {
					state.second.state = nextStateNum++;
					Dstates.push_back(state.second);
					// table[currState][currInput] = state
					table.second[stateSet.state][state.first] = state.second.state;

					continue;
				}

				// if the state has been found
				table.second[stateSet.state][state.first] = it->state;
			}

		}

	}

	void act_accept(StackType& stack, StateType& newState, std::pair<FSMStateSetType, DFATableType>& table) {
		std::cout << std::format("Accepted! the followpos set:\n{}", toString(followpos));

		// create Dstates and initialize it
		const auto& root = stack.back();
		const auto& rootFirstpos = root.data.data.firstpos;
		struct DFAState {
			std::set<Position> set;
			size_t state = 0;

			std::set<Position>::iterator begin() {
				return set.begin();
			}
			std::set<Position>::iterator end() {
				return set.end();
			}

			bool operator==(const DFAState& rhs) const {
				return set == rhs.set;
			}
			operator std::string() const {
				return std::format("[{}: {}]", state, m0st4fa::toString(set));
			}

			bool empty() const {
				return set.empty();
			}

			void insert(const Position& pos) {
				this->set.insert(pos);
			}
			void insert(const std::set<Position>& posSet) {
				this->set.insert(posSet.begin(), posSet.end());
			}
			bool contains(const Position& pos) const {
				return std::any_of(set.begin(), set.end(), [&pos](const Position& position) {
					return pos == position;
					});
			}
			bool contains(const size_t pos) const {
				return contains(Position{ pos, REGEX_END_MARKER });
			}
		};
		size_t nextStateNum = 2;
		std::vector<DFAState> Dstates{ {rootFirstpos, 1} };
		size_t markerPosition = allPos.back().position;
		std::cout << "marker position is: " << markerPosition << "\n";
		
		// fill Dstates and fill DTable
		_fill_table<DFAState>(Dstates, nextStateNum, markerPosition, table);
		
		// THE TRANSITION FUNCTION IS NOT COMPLETE
		std::for_each(Dstates.begin(), Dstates.end(), [markerPosition, &table](const DFAState& state) {
			if (state.contains(markerPosition)) {
				table.first.insert(state.state);
				std::cout << "\nFOUND FINAL\n";
			};
			});

		Logger logger;
		logger.log(LoggerInfo::INFO, std::format("{}\n{}\n{}", m0st4fa::toString(table.first), m0st4fa::toString(Dstates), m0st4fa::toString(table.second)));

	}

	void act_factor_alpha_num([[maybe_unused]] StackType& stack, StateType& newState) {
		// <factor> -> alpha | num

		auto& newStateData = newState.data.data;

		const StateType& alphaNum = stack.back();
		Position newPos = { currPos++, alphaNum.token.attribute.at(0) };
		newStateData.firstpos = { newPos };
		newStateData.lastpos = { newPos };
		newStateData.nullable = false;
		
		allPos.push_back(newPos);
	}

	void act_escape(StackType& stack, StateType& newState) {
		// <esq_seq> -> BACK_SLASH factor
		const auto& lastState = stack.back();
		auto& newStateData = newState.data.data;

		switch (lastState.token.name) {
			using enum m0st4fa::Terminal;
		case T_QUANT_STAR_LAZY: {
			Position newPos = { currPos++, '*' };
			newStateData.firstpos = { newPos };
			newStateData.lastpos = { newPos };
			newStateData.nullable = false;

			allPos.push_back(newPos);
			break;
		}
		default:
			act_pass_last(stack, newState);
		}

	}

	// <char_class_seq> -> <esq_seq><char_class_seq>
	void act_char_class_seq(StackType& stack, StateType& newState) {
		newState.data.data = stack.back().data.data;

		const auto& charClassSeqData = stack.back().data.data;
		const auto& esqSeqData = stack.at(stack.size() - 2).data.data;
		newState.data.data.insertFirstpos(esqSeqData.firstpos);
		newState.data.data.insertLastpos(esqSeqData.lastpos);

		auto calcFollow = [&charClassSeqData, &esqSeqData]() {
			const auto& fpSet = charClassSeqData.firstpos;
			const auto& lpSet = esqSeqData.lastpos;

			// ∀i⋳firstpos(charrClassSeq)
			std::ranges::for_each(fpSet.begin(), fpSet.end(), [&lpSet](const auto i) {
				// ∀j⋳lastpos(esqSeq)
				std::ranges::for_each(lpSet.begin(), lpSet.end(), [&i](const auto j) {
					// add j to followpos(i);
					followpos[i.position].insert(j);
					});
				}); };
		calcFollow();
	}

	// <char_class> -> [<char_class_seq>]
	// <char_class> -> [^<char_class_seq>]
	void act_char_class(StackType& stack, StateType& newState) {
		act_pass_prelast(stack, newState);

		// if it is the negated character class
		if (stack.at(stack.size() - 3).token.name == Terminal::T_LEFT_BRACKET_CHARET) {
			newState.data.data.fpAllPositions = newState.data.data.lpAllPositions = true;
		}
	}

	// <quant> -> <quant>op      // TODO: CONTINUE THE LAZY QUANTIFIERS
	void act_quant_op(StackType& stack, StateType& newState) {
		Terminal op = stack.back().token.name;
		const StateType& quant = stack.at(stack.size() - 2);
		const auto& quantData = quant.data.data;
		const auto& data = newState.data.data;
		act_pass_prelast(stack, newState);

		switch (op) {
			using enum m0st4fa::Terminal;
		case T_QUANT_STAR_GREEDY:
			newState.data.data.nullable = true;
			set_follow(data.firstpos, data.lastpos);
			break;
		case T_QUANT_PLUS_GREEDY:
			set_follow(data.firstpos, data.lastpos);
			break;
		case T_QUANT_QMARK_GREEDY:
			newState.data.data.nullable = true;
			break;
		case T_QUANT_STAR_LAZY:
			newState.data.data.nullable = true;  
			break;
		case T_QUANT_PLUS_LAZY:
			break;
		case T_QUANT_QMARK_LAZY:
			newState.data.data.nullable = true;
			break;
		default:
			throw std::exception("UNHANDLED OP IN `act_quant_op`. TODO: CHANGE THIS HANDLING.");
		}
	}

	// <quant> -> <quant>braces
	void act_quant_brace(StackType& stack, StateType& newState) { // TODO: CONTINUE THIS
		throw std::exception("act_quant_brace(): COMPLETE THIS FUNCTION");

		const auto& rBrace = stack.back();


		if (rBrace.token.name == Terminal::T_RIGHT_BRACE) {
			// <quant> -> <quant>{m, n}
			// <quant> -> <quant>{n,}
			// <quant> -> <quant>{n}



		}
		else { // rBrace.token.name == Terminal::T_RIGHT_BRACE_LAZY
			
		}

	}

	// <conc> -> <conc><quant>
	void act_conc(StackType& stack, StateType& newState) {
		const auto& quant = stack.back();
		const auto& quantData = quant.data.data;

		const auto& conc = stack.at(stack.size() - 2);
		const auto& concData = conc.data.data;

		auto& newData = newState.data.data;

		// if none is nullable or any is nullable
		newData.firstpos = concData.firstpos;
		newData.lastpos = quantData.lastpos;

		if (concData.nullable && quantData.nullable) {
			newData.insertFirstpos(quantData.firstpos);
			newData.insertLastpos(concData.lastpos);
		}
		else if (concData.nullable) {
			newData.insertFirstpos(quantData.firstpos);
		}
		else if (quantData.nullable) {
			newData.insertLastpos(concData.lastpos);
		}

		// set follow
		set_follow(quantData.firstpos, concData.lastpos);
	
	};

	// <anchor> -> ^<anchor>
	void act_anc_charet(StackType& stack, StateType& newState) { // TODO: CONTINUE THIS
		act_pass_last(stack, newState);
	};

	// <anchor> -> <anchor>?
	void act_anc_dollar(StackType& stack, StateType& newState) { // TODO: CONTINUE THIS
		act_pass_prelast(stack, newState);
	};

	// <regex> -> <regex> | <anchor>
	void act_or(StackType& stack, StateType& newState) {
		const auto& regex = stack.at(stack.size() - 3);
		const auto& regexData = regex.data.data;

		const auto& anchor = stack.back();
		const auto& anchorData = anchor.data.data;

		auto& newData = newState.data.data;
		newData.nullable = regexData.nullable or anchorData.nullable;

		newData.firstpos = regexData.firstpos;
		newData.insertFirstpos(anchorData.firstpos);

		newData.lastpos = regexData.lastpos;
		newData.insertLastpos(anchorData.lastpos);

	};

	GrammarType RegularExpression::_get_grammar() {
		GrammarType grammar;
		size_t index = 0;

		ProductionType prod;
		SymbolType vRegEx = toSymbol(Variable::V_REGEX);
		SymbolType vRegExp = toSymbol(Variable::V_REGEX_P);
		SymbolType vConc = toSymbol(Variable::V_CONC);
		SymbolType vQuant = toSymbol(Variable::V_QUANT);
		SymbolType vCharClass = toSymbol(Variable::V_CHAR_CLASS);
		SymbolType vCharClassSeq = toSymbol(Variable::V_CHAR_CLASS_SEQ);
		SymbolType vAnch = toSymbol(Variable::V_ANCHOR);
		SymbolType vEscapeSeq = toSymbol(Variable::V_ESQ_SEQ);
		SymbolType vGroup = toSymbol(Variable::V_GROUP);
		SymbolType vRange = toSymbol(Variable::V_RANGE);
		SymbolType vFact = toSymbol(Variable::V_FACTOR);

		SymbolType tOr = toSymbol(Terminal::T_OP_OR);
		SymbolType tAnchCharet = toSymbol(Terminal::T_ANC_CHARET);
		SymbolType tAnchDollar = toSymbol(Terminal::T_ANC_DOLLAR);
		SymbolType tAncA = toSymbol(Terminal::T_ANC_A);
		SymbolType tAncz = toSymbol(Terminal::T_ANC_z);
		SymbolType tAncZ = toSymbol(Terminal::T_ANC_Z);
		SymbolType tAncG = toSymbol(Terminal::T_ANC_G);
		SymbolType tBackSlash = toSymbol(Terminal::T_BACK_SLASH);
		SymbolType tEOF = toSymbol(Terminal::T_EOF);
		SymbolType tEpsilon = toSymbol(Terminal::T_EPSILON);
		SymbolType tLBracket = toSymbol(Terminal::T_LEFT_BRACKET);
		SymbolType tLParen = toSymbol(Terminal::T_LEFT_PAREN);
		SymbolType tLBrace = toSymbol(Terminal::T_LEFT_BRACE);
		SymbolType tRBracket = toSymbol(Terminal::T_RIGHT_BRACKET);
		SymbolType tLBracketCharet = toSymbol(Terminal::T_LEFT_BRACKET_CHARET);
		SymbolType tRParen = toSymbol(Terminal::T_RIGHT_PAREN);
		SymbolType tRBrace = toSymbol(Terminal::T_RIGHT_BRACE);
		SymbolType tRBraceLZ = toSymbol(Terminal::T_RIGHT_BRACE_LAZY);
		SymbolType tQNTG = toSymbol(Terminal::T_QUANT_NTIMES_GREEDY);
		SymbolType tQNTLZ = toSymbol(Terminal::T_QUANT_NTIMES_LAZY);
		SymbolType tQNTMG = toSymbol(Terminal::T_QUANT_NT_OR_MORE_GREEDY);
		SymbolType tQNTMLZ = toSymbol(Terminal::T_QUANT_NT_OR_MORE_LAZY);
		SymbolType tQNTMTG = toSymbol(Terminal::T_QUANT_NT_OR_MT_GREEDY);
		SymbolType tQNTMTLZ = toSymbol(Terminal::T_QUANT_NT_OR_MT_LAZY);
		SymbolType tQPG = toSymbol(Terminal::T_QUANT_PLUS_GREEDY);
		SymbolType tQPLZ = toSymbol(Terminal::T_QUANT_PLUS_LAZY);
		SymbolType tQQG = toSymbol(Terminal::T_QUANT_QMARK_GREEDY);
		SymbolType tQQLZ = toSymbol(Terminal::T_QUANT_QMARK_LAZY);
		SymbolType tQSG = toSymbol(Terminal::T_QUANT_STAR_GREEDY);
		SymbolType tQSLZ = toSymbol(Terminal::T_QUANT_STAR_LAZY);
		SymbolType tAlpha = toSymbol(Terminal::T_ALPHA);
		SymbolType tNum = toSymbol(Terminal::T_NUM);

		// <regex'> -> <regex>
		prod = ProductionType{
			{vRegExp},
			{vRegEx},
			index
		};
		prod.postfixAction = &act_accept;
		grammar.pushProduction(prod);

		// <regex> -> <regex> | <anchor>
		prod = ProductionType{
			{vRegEx},
			{vRegEx, tOr, vAnch},
			++index
		};
		prod.postfixAction = &act_or;
		grammar.pushProduction(prod);

		// <regex> -> <anchor>
		prod = ProductionType{
			{vRegEx},
			{vAnch},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <anchor> -> ^<anchor>
		prod = ProductionType{
			{vAnch},
			{tAnchCharet, vAnch},
			++index
		};
		prod.postfixAction = &act_anc_charet;
		grammar.pushProduction(prod);

		// <anchor> -> <anchor>$
		prod = ProductionType{
			{vAnch},
			{vAnch, tAnchDollar},
			++index
		};
		prod.postfixAction = &act_anc_dollar;
		grammar.pushProduction(prod);

		// <anchor> -> <chonc>
		prod = ProductionType{
			{vAnch},
			{vConc},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <conc> -> <conc><quant>
		prod = ProductionType{
			{vConc},
			{vConc, vQuant},
			++index
		};
		prod.postfixAction = &act_conc;
		grammar.pushProduction(prod);

		// <conc> -> <quant>
		prod = ProductionType{
			{vConc},
			{vQuant},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <quant> -> <quant>*
		prod = ProductionType{
			{vQuant},
			{vQuant, tQSG},
			++index
		};
		prod.postfixAction = &act_quant_op;
		grammar.pushProduction(prod);

		// <quant> -> <quant>*?
		prod = ProductionType{
			{vQuant},
			{vQuant, tQSLZ},
			++index
		};
		prod.postfixAction = &act_quant_op;
		grammar.pushProduction(prod);

		// <quant> -> <quant>+
		prod = ProductionType{
			{vQuant},
			{vQuant, tQPG},
			++index
		};
		prod.postfixAction = &act_quant_op;
		grammar.pushProduction(prod);

		// <quant> -> <quant>+?
		prod = ProductionType{
			{vQuant},
			{vQuant, tQPLZ},
			++index
		};
		prod.postfixAction = &act_quant_op;
		grammar.pushProduction(prod);

		// <quant> -> <quant>?
		prod = ProductionType{
			{vQuant},
			{vQuant, tQQG},
			++index
		};
		prod.postfixAction = &act_quant_op;
		grammar.pushProduction(prod);

		// <quant> -> <quant>??
		prod = ProductionType{
			{vQuant},
			{vQuant, tQQLZ},
			++index
		};
		prod.postfixAction = &act_quant_op;
		grammar.pushProduction(prod);

		// <quant> -> <quant>{int m, int n}
		prod = ProductionType{
			{vQuant},
			{vQuant, tLBrace, tNum, tAlpha, tNum, tRBrace},
			++index
		};
		prod.postfixAction = &act_quant_brace;
		grammar.pushProduction(prod);

		// <quant> -> <quant>{int m, int n}?
		prod = ProductionType{
			{vQuant},
			{vQuant, tLBrace, tNum, tAlpha, tNum, tRBraceLZ},
			++index
		};
		prod.postfixAction = &act_quant_brace;
		grammar.pushProduction(prod);

		// <quant> -> <quant>{int n,}
		prod = ProductionType{
			{vQuant},
			{vQuant, tLBrace, tNum, tAlpha, tRBrace},
			++index
		};
		prod.postfixAction = &act_quant_brace;
		grammar.pushProduction(prod);

		// <quant> -> <quant>{int n,}?
		prod = ProductionType{
			{vQuant},
			{vQuant, tLBrace, tNum, tAlpha, tRBraceLZ},
			++index
		};
		prod.postfixAction = &act_quant_brace;
		grammar.pushProduction(prod);

		// <quant> -> <quant>{int n}
		prod = ProductionType{
			{vQuant},
			{vQuant, tLBrace, tNum, tRBrace},
			++index
		};
		prod.postfixAction = &act_quant_brace;
		grammar.pushProduction(prod);

		// <quant> -> <quant>{int n}?
		prod = ProductionType{
			{vQuant},
			{vQuant, tLBrace, tNum, tRBraceLZ},
			++index
		};
		prod.postfixAction = &act_quant_brace;
		grammar.pushProduction(prod);

		// <quant> -> <group>
		prod = ProductionType{
			{vQuant},
			{vGroup},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <group> -> (<regex>)
		prod = ProductionType{
			{vGroup},
			{tLParen, vRegEx, tRParen},
			++index
		};
		prod.postfixAction = &act_pass_prelast;
		grammar.pushProduction(prod);

		// <group> -> <char_class>
		prod = ProductionType{
			{vGroup},
			{vCharClass},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <char_class> -> [<char_class_seq>]
		prod = ProductionType{
			{vCharClass},
			{tLBracket, vCharClassSeq, tRBracket},
			++index
		};
		prod.postfixAction = &act_char_class;
		grammar.pushProduction(prod);

		// <char_class> -> [^<char_class_seq>]
		prod = ProductionType{
			{vCharClass},
			{tLBracketCharet, vCharClassSeq, tRBracket},
			++index
		};
		prod.postfixAction = &act_char_class;
		grammar.pushProduction(prod);

		// <char_class> -> <esq_seq>
		prod = ProductionType{
			{vCharClass},
			{vEscapeSeq},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		//// <char_class_seq> -> <range><char_class_seq>
		//prod = ProductionType{
		//	{vCharClassSeq},
		//	{vRange, vCharClassSeq},
		//	++index
		//};
		//grammar.pushProduction(prod);

		// <char_class_seq> -> <esq_seq><char_class_seq>
		prod = ProductionType{
			{vCharClassSeq},
			{vEscapeSeq, vCharClassSeq},
			++index
		};
		prod.postfixAction = &act_char_class_seq;
		grammar.pushProduction(prod);

		// <char_class_seq> -> <esq_seq>
		prod = ProductionType{
			{vCharClassSeq},
			{vEscapeSeq},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <range> -> <factor>-<factor>
		//prod = ProductionType{
		//	{vRange},
		//	{vFact},
		//	++index
		//};
		//grammar.pushProduction(prod);

		// <esq_seq> -> \<factor>
		prod = ProductionType{
			{vEscapeSeq},
			{tBackSlash, vFact},
			++index
		};
		prod.postfixAction = &act_escape;
		grammar.pushProduction(prod);

		// TODO: add <esq_seq> -> \x for every quantifier and special lexeme x
		// <esq_seq> -> \*
		prod = ProductionType{
			{vEscapeSeq},
			{tBackSlash, tQSLZ},
			++index
		};
		prod.postfixAction = &act_escape;
		grammar.pushProduction(prod);

		// <esq_seq> -> <factor>
		prod = ProductionType{
			{vEscapeSeq},
			{vFact},
			++index
		};
		prod.postfixAction = &act_pass_last;
		grammar.pushProduction(prod);

		// <factor> -> ALPHA | NUM
		prod = ProductionType{
			{vFact},
			{tAlpha},
			++index
		};
		prod.postfixAction = &act_factor_alpha_num;
		grammar.pushProduction(prod);

		prod = ProductionType{
			{vFact},
			{tNum},
			++index
		};
		prod.postfixAction = &act_factor_alpha_num;
		grammar.pushProduction(prod);

		return grammar;
	}
}
#pragma once
#include <vector>

#include "Parser.h"

namespace m0st4fa {

	template <typename GrammarT, typename LexicalAnalyzerT,
		typename SymbolT, typename StateT,
		// TODO: ADD A TEMPLATE PARAMETER FOR THE STACK ELEMENT TYPE (DataT or StaclElementT)
		typename ParsingTableT, 
		typename FSMTableT = FSMTable<>,
		typename InputT = std::string>
	class LRParser : public Parser<LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT> {
		using ParserBase = Parser<LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT>;

		using ProductionType = decltype(GrammarT{}.at(0));
		using SymbolType = decltype(ProductionType{}.prodHead);
		using TerminalType = decltype(SymbolType{}.as.terminal);
		using VariableType = decltype(SymbolType{}.as.nonTerminal);

		using StackElementType = StateT;
		using DataType = decltype(StackElementType{}.data);
		using StackType = StackType<StackElementType>;

		using TokenType = decltype(LexicalAnalyzerT{}.getNextToken());

		mutable StackType m_Stack{0};
		mutable StackElementType m_CurrTopState{ START_STATE };
		mutable TokenType m_CurrInputToken{ TokenType{} };

		void _reset_parser_state() const {
			this->m_Logger.log(LoggerInfo::INFO, "RESETTING PARSER.");
			m_Stack.clear();
			m_CurrTopState = START_STATE;
			m_CurrInputToken = TokenType{};
		}

		void _reduce(size_t);

		void _push_state(const StateT& state) {

			this->m_CurrTopState = state;
			this->m_Stack.push_back(state);

			this->m_Logger.log(LoggerInfo::INFO, std::format("Pushing state {}\nCurrent stack: {}", (std::string)state, stringfy(this->m_Stack)));
		}

		void _pop_state() {

			if (this->m_Stack.size() <= 1) {

				std::string msg = std::format("Cannot pop more states from the LR stack. The stack cannot reach an empty stated.");

				this->m_Logger.log(LoggerInfo::ERR_STACK_UNDERFLOW, msg);

				throw std::runtime_error((std::string)"Stack underflow: " + msg);
			}
			
			this->m_CurrTopState = this->m_Stack.back();
			this->m_Logger.log(LoggerInfo::INFO, std::format("Popping state {}\nCurrent stack: {}", (std::string)this->m_CurrTopState, stringfy(this->m_Stack)));

			this->m_Stack.pop_back();
		}

		void _pop_states(size_t num) {

			if (this->m_Stack.size() < num + 1) {
				std::string msg = std::format("Cannot pop {} states from the LR stack. The stack cannot reach an empty stated.", num);

				this->m_Logger.log(LoggerInfo::ERR_STACK_UNDERFLOW, msg);

				throw std::runtime_error((std::string)"Stack underflow: " + msg);
			}

			const auto end = this->m_Stack.end();

			StackType temp{ end - num, end };

			this->m_Stack.erase(end - num, end);
			this->m_Logger.log(LoggerInfo::INFO, std::format("Popping states {}\nCurrent stack: {}", stringfy(temp), stringfy(this->m_Stack)));

			this->m_CurrTopState = this->m_Stack.back();
		}

		void _error_recov_panic_mode() const {

		}

	protected:
		static const StateT START_STATE;

	public:
		
		LRParser() = default;
		LRParser(const LexicalAnalyzerT& lexer,
			const ParsingTableT& parsingTable,
			const SymbolT& startSymbol) : ParserBase{lexer, parsingTable, startSymbol} {};
		ParserResult parse(ErrorRecoveryType = ErrorRecoveryType::ERT_NONE);
		
	};

	template<typename GrammarT, typename LexicalAnalyzerT, typename SymbolT, typename StateT, typename ParsingTableT, typename FSMTableT, typename InputT>
	const StateT LRParser<GrammarT, LexicalAnalyzerT, SymbolT, StateT, ParsingTableT, FSMTableT, InputT>::START_STATE{ 0 };

	// IMPLEMENTATION
	
	template<typename GrammarT, typename LexicalAnalyzerT, typename SymbolT, typename StateT, typename ParsingTableT, typename FSMTableT, typename InputT>
	inline void LRParser<GrammarT, LexicalAnalyzerT, SymbolT, StateT, ParsingTableT, FSMTableT, InputT>::_reduce(size_t prodNumber)
	{
		const std::string& src = this->getLexicalAnalyzer().getSourceCode();

		// get the production
		const auto& production = this->m_Table.grammar.at(prodNumber);

		StackElementType newState = StackElementType{};

		// exeucte the action, if any
		void(*action)(StackType&, StackElementType&) = static_cast<void(*)(StackType&, StackElementType&)>(production.postfixAction);

		if (action != nullptr)
			action(this->m_Stack, newState);

		// determine the length of the body of the production
		const size_t prodBodyLength = production.prodBody.size();

		// pop prodBodyLength elements from the top of the stack and get the next entry
		this->_pop_states(prodBodyLength);
		size_t stateNum = this->m_CurrTopState.state;
		LRTableEntry currEntry = this->m_Table.atGoto(stateNum, production.prodHead.as.nonTerminal);
		newState.state = currEntry.number;

		// Note: errors are never detected when consulting the GOTO table
		// this here is just a precaution for possible (probably logic) bugs
		if (currEntry.type != LRTableEntryType::TET_GOTO) {
			std::string msg{ std::format("Incorrect entry type! Expected type `GOTO` within function reduce after accessing the GOTO table.\nCurrent stack: {}\n Current input: {}", stringfy(this->m_Stack), src) };
			this->m_Logger.log(LoggerInfo::ERR_INVALID_VAL, msg);

			throw std::logic_error("Incorrect entry type!Expected type `GOTO` within function reduce after accessing the GOTO table.");
		}

		// if the current entry is not an error
		this->_push_state(newState);
	};

	template<typename GrammarT, typename LexicalAnalyzerT, typename		SymbolT, typename StateT, 
		typename ParsingTableT, 
		typename FSMTableT, typename InputT>
	ParserResult LRParser<GrammarT, LexicalAnalyzerT, 
		SymbolT, StateT, 
		ParsingTableT, FSMTableT, InputT>::
		parse(ErrorRecoveryType errorRecoveryType)
	{
		ParserResult result{};

		/** Algorithm
		* Initialize the stack to contain only the start state.
		* Loop inifinitly until: the parser accepts or an error is produced.
		* For every state I on top of the stack and token T from the input:
			* A = ACTION[I][T.asTerminal] and switch on A:
				* If A == SHIFT J: 
					* Push J on top of the stack and get next input.
				* Else if A == REDUCE J:
					* Call _reduce(J).
				* Else ERROR.

		* _reduce(J):
			* Get to production P indexed J and do the following:
				* If P contains an action, execute the action before reduction.
				* Get the length L of the production body, and remove from top of the stack L states.
				* Assume the state on top of the stack is now state I, and the head of production J is nonterminal H:
					* Push GOTO[I][H.as.nonTerminal] on top of the stack.
				* Check for whether the new top state is an error.
		* _error_recovery(errorRecoveryType): TBD.
		*/

		this->_reset_parser_state();
		this->_push_state(START_STATE);
		this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
		TerminalType currTokenName = this->m_CurrInputToken.name;

		while (true) {

			size_t currStateNum = this->m_CurrTopState.state;
			LRTableEntry currEntry = this->m_Table.atAction(currStateNum, currTokenName);
			const std::string& src = this->getLexicalAnalyzer().getSourceCode();

			// if we detect an error
			if (currEntry.isEmpty || currEntry.type == LRTableEntryType::TET_ERROR) {

				if (currEntry.isEmpty) {
					std::string msg{ std::format("LR parsing table entry is empty!\nCurrent stack: {}\n Current input: {}", stringfy(this->m_Stack), src) };
					this->m_Logger.log(LoggerInfo::ERR_INVALID_VAL, msg);
				}

				// if error recovery is not enabled
				if (errorRecoveryType == ErrorRecoveryType::ERT_NONE) {
					std::string msg = std::format("Cannot continue further with the parse! Error entry encountered; It looks like this string does not belong to the grammar.\nCurrent stack: {}\n Current input: {}", stringfy(this->m_Stack), src);
					this->m_Logger.log(LoggerInfo::ERR_UNACCEPTED_STRING, msg);

					throw std::logic_error("Cannot continue further with the parse! Error entry encountered; It looks like this string does not belong to the grammar.");
				}

				// if error recovery is enabled, switch on the type
				switch (errorRecoveryType) {
				case ErrorRecoveryType::ERT_PANIC_MODE: {
					std::string msg = "ERT_PANIC_MODE in progress!";
					throw std::logic_error(msg);
					break;
				}
				default: {
					std::string errMsg = std::format("Cannot continue further with the parse! Error entry encountered; It looks like this string does not belong to the grammar.\nCurrent stack: {}\n Current input: {}", stringfy(this->m_Stack), src);
					std::string noteMsg = std::format("Error recovery type `{}` is not yet supported for LR parsing.", stringfy(errorRecoveryType));
					std::string fullMsg = std::format("{}\nNote: ", errMsg, noteMsg);

					this->m_Logger.log(LoggerInfo::ERR_UNACCEPTED_STRING, fullMsg);
					throw std::logic_error("Cannot continue further with the parse! Error entry encountered; It looks like this string does not belong to the grammar.");
				}
				}
				
			}

			switch (currEntry.type)
			{
			case LRTableEntryType::TET_ACTION_SHIFT: {
				StateT s = StateT{ currEntry.number };
				s.token = this->m_CurrInputToken;
				this->_push_state(s);
				this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
				currTokenName = this->m_CurrInputToken.name;
				break;
			}

			case LRTableEntryType::TET_ACTION_REDUCE:
				_reduce(currEntry.number);
				break;

			case LRTableEntryType::TET_ACCEPT:
				std::cout << "\nACCEPTED\n";
				return result;

			default:
				this->m_Logger.log(LoggerInfo::FATAL_ERROR, std::format("[Unreachable] Invalid entry type on switch statement!\nCurrent stack: {}\n Current input: {}", stringfy(this->m_Stack), src));
				std::string srcLoc = this->m_Logger.getCurrSourceLocation();
				assert(std::format("Source code location:\n{}", srcLoc).data());
				break;
			}

		}

		return result;
	}

}

namespace m0st4ta {


}
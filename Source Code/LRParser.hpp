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
			m_Stack.clear();
			m_CurrTopState = START_STATE;
			m_CurrInputToken = TokenType{};
		}

		void _reduce(size_t);

		void _push_state(const StateT& state) {
			this->m_CurrTopState = state;
			this->m_Stack.push_back(state);
		}

		void _pop_state() {

			if (this->m_Stack.size() <= 1) {
				assert("TODO: handle poping elements from the stack when it has size zero");
			}

			this->m_CurrTopState = this->m_Stack.back();
			this->m_Stack.pop_back();
		}

		void _pop_states(size_t num) {

			if (this->m_Stack.size() < num + 1) {
				assert("TODO: handle poping elements from the stack when it has insufficient elements && the stack cannot be empty after popping from it");
			}

			const auto end = this->m_Stack.end();
			this->m_Stack.erase(end - num, end);

			this->m_CurrTopState = this->m_Stack.back();
		}

	protected:
		static constexpr const StateT START_STATE{ 0, DataType{} };

	public:
		
		LRParser() = default;
		LRParser(const LexicalAnalyzerT& lexer,
			const ParsingTableT& parsingTable,
			const SymbolT& startSymbol) : ParserBase{lexer, parsingTable, startSymbol} {};
		ParserResult parse(ErrorRecoveryType = ErrorRecoveryType::ERT_NONE);
		
	};

	/*template<typename GrammarT, typename LexicalAnalyzerT, typename SymbolT, typename StateT, typename ParsingTableT, typename FSMTableT, typename InputT>
	const StateT LRParser<GrammarT, LexicalAnalyzerT, SymbolT, StateT, ParsingTableT, FSMTableT, InputT>::START_STATE { 0, 0 };
	*/
	// IMPLEMENTATION
	
	template<typename GrammarT, typename LexicalAnalyzerT, typename SymbolT, typename StateT, typename ParsingTableT, typename FSMTableT, typename InputT>
	inline void LRParser<GrammarT, LexicalAnalyzerT, SymbolT, StateT, ParsingTableT, FSMTableT, InputT>::_reduce(size_t prodNumber)
	{
		// get the production
		const auto& production = this->m_Table.grammar.at(prodNumber);

		// exeucte the action, if any
		void(*action)(StackType&, StackElementType&) = static_cast<void(*)(StackType&, StackElementType&)>(production.postfixAction);

		if (action != nullptr)
			action(this->m_Stack, this->m_CurrTopState);

		// determine the length of the body of the production
		const size_t prodBodyLength = production.prodBody.size();

		// pop prodBodyLength elements from the top of the stack and get the next entry
		this->_pop_states(prodBodyLength);
		size_t stateNum = this->m_CurrTopState.state;
		LRTableEntry currEntry = this->m_Table.atGoto(stateNum, production.prodHead.as.nonTerminal);

		if (currEntry.type != LRTableEntryType::TET_GOTO) {
			assert("TODO: the current LR table entry is an error. this is a temporary handle of such an error and a more sofisticated one should be created!");
		}

		// if the current entry is not an error
		this->_push_state(StackElementType{ currEntry.number });
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
				* Else A == ERROR:
					* Call _error_recovery(errorRecoveryType).

		* _reduce(J):
			* Get to production P indexed J and do the following:
				* If P contains an action, execute the action before reduction.
				* Get the length L of the production body, and remove from top of the stack L states.
				* Assume the state on top of the stack is now state I, and the head of production J is nonterminal H:
					* Push GOTO[I][H.as.nonTerminal] on top of the stack.
				* Check for whether the new top state is an error.
		* _error_recovery(errorRecoveryType): TBD.
		*/

		this->_push_state(START_STATE);
		this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
		TerminalType currTokenName = this->m_CurrInputToken.name;

		while (true) {

			size_t currStateNum = this->m_CurrTopState.state;
			
			LRTableEntry currEntry = this->m_Table.atAction(currStateNum, currTokenName);
			
			// TODO: CHECK THE ENTRY IS NOT EMPTY

			// TODO: HANDLE CASES WHERE THE ENTRY IS AN ERROR ENTRY
			if (currEntry.type == LRTableEntryType::TET_ERROR) {
				assert("TODO: the current LR table entry is an error. this is a temporary handle of such an error and a more sofisticated one should be created!");
			}

			switch (currEntry.type)
			{
			case LRTableEntryType::TET_ACTION_SHIFT: {
				this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
				currTokenName = this->m_CurrInputToken.name;
				this->_push_state(StateT{ currEntry.number });
				break;
			}

			case LRTableEntryType::TET_ACTION_REDUCE:
				_reduce(currEntry.number);
				break;

			case LRTableEntryType::TET_ACCEPT:
				std::cout << "\nACCEPTED\n";
				return result;

			default:
				// TODO [UNREACHABLE]: HANDLE CASES WHERE THE COUNT IS THE TYPE OF THE ENTRY
				assert("Unreachable!");
				break;
			}

		}

		return result;
	}

}

namespace m0st4ta {


}
#pragma once
#include <vector>

#include "Parser.h"

namespace m0st4fa {

	template <typename GrammarT, typename LexicalAnalyzerT,
		typename SymbolT,
		// TODO: ADD A TEMPLATE PARAMETER FOR THE STACK ELEMENT TYPE (DataT or StaclElementT)
		typename ParsingTableT, 
		typename FSMTableT = FSMTable<>,
		typename InputT = std::string>
	class LRParser : public Parser<LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT> {
		using ParserBase = Parser<LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT>;
		using StackElementType = LRParserState<>;
		using ParserStackType = std::vector<StackElementType>;
		using TokenType = decltype(LexicalAnalyzerT{}.getNextToken());

		mutable ParserStackType m_Stack{10};
		mutable StackElementType m_CurrTopState{ START_STATE };
		mutable TokenType m_CurrInputToken{ TokenType{} };

		void _reset_parser_state() const {
			m_Stack.clear();
			m_CurrTopState = START_STATE;
			m_CurrInputToken = TokenType{};
		}

		void _reduce(StackElementType&);

	protected:
		static constexpr const StackElementType START_STATE = ParserStackType{0};

	public:
		
		LRParser() = default;
		ParserResult parse(ErrorRecoveryType = ErrorRecoveryType::ERT_NONE);
		
	};

	
	// IMPLEMENTATION
	
	template<typename GrammarT, typename LexicalAnalyzerT, typename SymbolT, typename ParsingTableT, typename FSMTableT, typename InputT>
	inline void LRParser<GrammarT, LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT>::_reduce(StackElementType& state)
	{
		// get the production
		const auto& production = this->m_Table.grammar.at(state.state);

		// exeucte the action, if any
		void(*action)(StackType&, StackElementType&) = static_cast<void(*)(StackType&, StackElementType&)>(production.postfixAction);
		action(this->m_Stack, this->m_CurrTopState);

		// determine the length of the body of the production
		const size_t prodBodyLength = production.prodBody.size();

		// pop prodBodyLength elements from the top of the stack
		const auto stackEnd = this->m_Stack.end();
		this->m_Stack.erase(stackEnd - prodBodyLength - 1, stackEnd);

		// get the state to be pushed on top of the stack
		this->m_CurrTopState = this->m_Stack.back();
		size_t stateNum = this->m_CurrTopState.state;
		LRTableEntry currEntry = this->m_Table[stateNum][this->m_CurrInputToken.name];

		if (currEntry.type == LRTableEntryType::TET_ERROR) {
			assert("TODO: the current LR table entry is an error. this is a temporary handle of such an error and a more sofisticated one should be created!");
		}

		// if the current entry is not an error
		StackElementType currState = StackElementType{currEntry.number};
		this->m_Stack.push_back(currState);
	};

	template<typename GrammarT, typename LexicalAnalyzerT, typename SymbolT, typename ParsingTableT, typename FSMTableT, typename InputT>
	ParserResult LRParser<GrammarT, LexicalAnalyzerT, SymbolT, ParsingTableT, FSMTableT, InputT>::parse(ErrorRecoveryType errorRecoveryType)
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

		this->m_Stack.push_back(START_STATE);
		this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();

		while (true) {

			this->m_CurrTopState = this->m_Stack.back();
			this->m_Stack.pop_back();
			
			LRTableEntry currEntry = this->m_Table[this->m_CurrTopState][this->m_CurrInputToken];
			
			// TODO: CHECK THE ENTRY IS NOT EMPTY

			// TODO: HANDLE CASES WHERE THE ENTRY IS AN ERROR ENTRY
			if (currEntry.type == LRTableEntryType::TET_ERROR) {
				assert("TODO: the current LR table entry is an error. this is a temporary handle of such an error and a more sofisticated one should be created!");
			}

			switch (currEntry.type)
			{
			case LRTableEntryType::TET_ACTION_SHIFT:
				LRState newState = LRState{ currEntry.number };
				this->m_Stack.push_back(newState);
				break;

			case LRTableEntryType::TET_ACTION_REDUCE:
				_reduce(currEntry.number);
				break;

			case LRTableEntryType::TET_GOTO:
				break;

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
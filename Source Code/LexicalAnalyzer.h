#pragma once

#include "FiniteStateMachine.h"
#include "DFA.h"
#include "Logger.h"

#include <string>


namespace m0st4fa {
	
	template<typename TokenT, typename InputT = std::string>
	//                               state    lexeme
	using TokenFactoryT = TokenT (*)(state_t, InputT);
	
	template <typename TokenT, typename TableT, typename InputT = std::string>
	class LexicalAnalyzer {

		DFA<TransFn<TableT>, InputT> m_Automatan;
		TokenFactoryT<TokenT, InputT> m_TokenFactory = nullptr;
		std::string m_SourceCode;

		size_t m_BeginPtr = 0,
			m_ForwardPtr = 0,
			m_Line = 1,
			m_Col = 1;

		Logger m_Logger;


		// methods
		void _remove_whitespace(void);
	protected:

		const DFA<TransFn<TableT>, InputT>& getAutomatan() { return this->m_Automatan; };
		const TokenFactoryT<TokenT, InputT> getTokenFactory() { return this->m_TokenFactory; };
		const std::string& getSourceCode() { return this->m_SourceCode; };
		size_t getBeginPtr() { return this->m_BeginPtr; };
		size_t getForwardPtr() { return this->m_ForwardPtr; };

	public:

		LexicalAnalyzer(
			DFA<TransFn<TableT>, InputT> automaton,
			TokenFactoryT<TokenT, InputT> tokenFactory,
			std::string sourceCode) :
			m_Automatan{automaton}, m_TokenFactory{ tokenFactory }, m_SourceCode{ sourceCode }
		{

			LoggerInfo info;
			info.level = LOG_LEVEL::LL_ERROR;
			info.info = { .errorType = ERROR_TYPE::ET_INVALID_ARGUMENT };
			
			if (this->m_TokenFactory == nullptr) {
				this->m_Logger.log(info, "TokenFactory is not set");
				throw std::runtime_error("TokenFactory is not set");
			};

		}
		
		TokenT getNextToken();
		size_t getLine() { return this->m_Line; };
		size_t getCol() { return this->m_Col; };
		std::pair<size_t, size_t> getPosition() {
			return std::pair<size_t, size_t>{m_Line, m_Col};
		};
		
	};

	
}

namespace m0st4fa {
	template<typename TokenT, typename TableT, typename InputT>
	void LexicalAnalyzer<TokenT, TableT, InputT>::_remove_whitespace(void)
	{

		// remove all whitespaces
		while (true) {
			char currChar = *this->m_SourceCode.data();
			bool isWhiteSpace = std::isspace(currChar);

			if (isWhiteSpace) {
				// erase whitespace from source code stream
				this->m_SourceCode.erase(this->m_SourceCode.begin());

				// update line and col
				if (currChar == '\n')
					this->m_Line++, this->m_Col = 1;
				else
					this->m_Col++;

			}
			else
				break;
			
		}
		
		return;
	}

	template<typename TokenT, typename TableT, typename InputT>
	TokenT m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::getNextToken()
	{

		// if we are at the end of the source code or it is empty, return EOF token
		if (this->m_SourceCode.empty()) {
			LoggerInfo info;
			info.level = LOG_LEVEL::LL_DEBUG;

			std::string msg = std::format("({}, {}) {:s}", this->m_Line, this->m_Col, std::string{"End of file reached"});
			this->m_Logger.log(info, msg);

			// assuming that EOF is the default value for a token
			return TokenT{};
		};
		
		TokenT res{};

		// remove all whitespaces and count new lines
		this->_remove_whitespace();

		// get the lexeme and the final state reached (if any)
		const FSMResult fsmRes = this->m_Automatan.simulate(this->m_SourceCode, FSM_MODE::MM_LONGEST_PREFIX);
		const state_t fstate = *fsmRes.finalState.begin();

		// check whether there is a matched lexeme
		if (-not fsmRes.accepted) {
			
			LoggerInfo info;
			info.level = LOG_LEVEL::LL_ERROR;
			info.info = { .errorType = ERROR_TYPE::ET_INVALID_LEXEME };

			std::string msg = std::format("({}, {}) {:s}", this->m_Line, this->m_Col, std::string{"No lexeme matched"});
			this->m_Logger.log(info, msg);
			throw std::runtime_error("No lexeme accepted by the state machine");

		}
		
		// if a lexeme is accepted, extract it
		const size_t lexemeSize = fsmRes.indecies.end;
		this->m_Col += lexemeSize;
		const std::string lexeme = this->m_SourceCode.substr(0, lexemeSize);

		// get the token
		res = m_TokenFactory(fstate, lexeme);

		// erease the lexeme from source code stream
		m_SourceCode.erase(0, lexemeSize);

		return res;
	}

	
}
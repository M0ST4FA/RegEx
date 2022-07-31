#pragma once

#include "FiniteStateMachine.h"
#include "DFA.h"
#include "Logger.h"

#include <string>


namespace m0st4fa {
	
	template<typename TokenT, typename InputT>
	//                               state    lexeme
	using TokenFactoryT = TokenT (*)(state_t, InputT);
	
	template <typename TokenT, typename InputT = std::string>
	class LexicalAnalyzer {

		DFA<TransFn<InputT>, InputT> m_Automatan;
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

		const DFA<TransFn<InputT>, InputT>& getAutomatan() { return this->m_Automatan; };
		const TokenFactoryT<TokenT, InputT> getTokenFactory() { return this->m_TokenFactory; };
		const std::string& getSourceCode() { return this->m_SourceCode; };
		size_t getBeginPtr() { return this->m_BeginPtr; };
		size_t getForwardPtr() { return this->m_ForwardPtr; };

	public:

		LexicalAnalyzer(
			DFA<TransFn<InputT>, InputT> automaton,
			TokenFactoryT<TokenT, InputT> tokenFactory,
			std::string sourceCode) :
			m_Automatan{automaton}, m_TokenFactory{ tokenFactory }, m_SourceCode{ sourceCode }
		{

			LoggerInfo info;
			info.level = LOG_LEVEL::LL_ERROR;
			
			if (this->m_TokenFactory == nullptr) {
				this->m_Logger.log(info, "TokenFactory is not set");
				throw std::runtime_error("TokenFactory is not set");
			};

		}
		
		TokenT getNextToken();
		size_t getLine() { return this->m_Line; };
		size_t getCol() { return this->m_Col; };
		
	};

	
}

namespace m0st4fa {
	template<typename TokenT, typename InputT>
	void LexicalAnalyzer<TokenT, InputT>::_remove_whitespace(void)
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

	template<typename TokenT, typename InputT>
	TokenT m0st4fa::LexicalAnalyzer<TokenT, InputT>::getNextToken()
	{
#define _EOF this->m_SourceCode.size();

		TokenT res{};
		
		// remove all whitespaces
		this->_remove_whitespace();
			
		// get the lexeme
		FSMResult fsmRes = this->m_Automatan.simulate(this->m_SourceCode, FSM_MODE::MM_LONGEST_PREFIX);
		size_t lexemeSize = fsmRes.indecies.end;
		std::string lexeme = this->m_SourceCode.substr(0, lexemeSize);

		res = m_TokenFactory();
		

		// erease the lexeme from source code stream
		m_SourceCode.erase(0, lexemeSize);

		return res;

#undef _EOF
	}

	
}
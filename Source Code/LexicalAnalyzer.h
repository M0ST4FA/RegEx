#pragma once

#include <string>

#include "FiniteStateMachine.h"
#include "DFA.h"
#include "Logger.h"
#include "LADataStructs.h"



namespace m0st4fa {

	template <typename TokenT, typename TableT = FSMTable<>, typename InputT = std::string>
	class LexicalAnalyzer {

		DFA<TransFn<TableT>, InputT> m_Automatan;
		TokenFactoryT<TokenT, InputT> m_TokenFactory = nullptr;
		std::string_view m_SourceCode;

		size_t m_Line = 1,
			/** When does the line number change?
			* When escaping a whitespace character (+1).
			* After matching a lexeme (+sizeof(lexeme)).
			*/
			   m_Col = 0;

		Logger m_Logger;


		// methods
		inline void _remove_whitespace(unsigned = (unsigned)LA_FLAG::LAF_NONE);
		inline bool _check_source_code_empty() {
			if (!this->m_SourceCode.empty())
				return false;

			std::string msg = std::format("({}, {}) {:s}", this->m_Line, this->m_Col, std::string{"End of file reached"});
			this->m_Logger.logDebug(msg);

			// assuming that EOF is the default value for a token
			return true;
		};
		inline void _check_accepted_fsm(const FSMResult& fsmRes) {
			if (fsmRes.accepted)
				return;

			LoggerInfo info;
			info.level = LOG_LEVEL::LL_ERROR;
			info.info = { .errorType = ERROR_TYPE::ET_INVALID_LEXEME };

			std::string msg = std::format("({}, {}) {:s}", this->m_Line, this->m_Col, std::string{"No lexeme matched"});
			this->m_Logger.log(info, msg);
			throw std::runtime_error("No lexeme accepted by the state machine");
		}
	protected:

		const DFA<TransFn<TableT>, InputT>& getAutomatan() { return this->m_Automatan; };
		const TokenFactoryT<TokenT, InputT> getTokenFactory() { return this->m_TokenFactory; };

	public:

		LexicalAnalyzer() = default;

		LexicalAnalyzer(
			DFA<TransFn<TableT>, InputT> automaton,
			TokenFactoryT<TokenT, InputT> tokenFactory,
			std::string_view sourceCode) :
			m_Automatan{ automaton }, m_TokenFactory{ tokenFactory }, m_SourceCode{ sourceCode }
		{

			if (this->m_TokenFactory == nullptr) {
				this->m_Logger.log(LoggerInfo::ERR_INVALID_ARG, "TokenFactory is not set");
				throw std::runtime_error("TokenFactory is not set");
			};

		}

		LexicalAnalyzer(const LexicalAnalyzer&) = default;
		LexicalAnalyzer(LexicalAnalyzer&&) = default;
		LexicalAnalyzer& operator= (const LexicalAnalyzer& rhs) {
			this->m_Automatan = rhs.m_Automatan;
			this->m_Col = rhs.m_Col;
			this->m_Line = rhs.m_Line;
			this->m_SourceCode = rhs.m_SourceCode;
			this->m_TokenFactory = rhs.m_TokenFactory;

			return *this;
		}

		TokenT getNextToken(unsigned = (unsigned)LA_FLAG::LAF_NONE);
		const std::string_view& getSourceCode() { return this->m_SourceCode; };
		TokenT peak(unsigned = (unsigned)LA_FLAG::LAF_NONE);
		size_t getLine() { return this->m_Line; };
		size_t getCol() { return this->m_Col; };
		std::pair<size_t, size_t> getPosition() {
			return std::pair<size_t, size_t>{m_Line, m_Col};
		};

	};


}

namespace m0st4fa {
	template<typename TokenT, typename TableT, typename InputT>
	void LexicalAnalyzer<TokenT, TableT, InputT>::_remove_whitespace(unsigned flags)
	{

		// this function will be entered in case white space characters are to be removed.
		// remove all whitespaces...
		while (true) {
			char currChar = *this->m_SourceCode.data();
			bool isWhiteSpace = std::isspace(currChar) && currChar != '\0'; // null marks the EOF.

			// if we've catched a white space...
			if (isWhiteSpace) {

				// if the current character is a new line char and they are allowed, do not remove the current char
				if (currChar == '\n' && (flags & (unsigned)LA_FLAG::LAF_ALLOW_NEW_LINE)) {
					this->m_Line++, this->m_Col = 0;
					this->m_SourceCode.remove_prefix(1);
					continue;
				}

				// if this whitespace character is not a new line character
				
				// erase whitespace from source code stream
				this->m_SourceCode.remove_prefix(1);

				// update character
				this->m_Col++;

			}
			// if no white space is caught
			else
				break;

		}

		return;
	}


	template<typename TokenT, typename TableT, typename InputT>
	TokenT m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::getNextToken(unsigned flags)
	{

		// remove all white spaces and count new lines
		if (!(flags & (unsigned)LA_FLAG::LAF_ALLOW_WHITE_SPACE_CHARS))
			this->_remove_whitespace(flags);

		// if we are at the end of the source code or it is empty, return EOF token
		if (this->_check_source_code_empty())
			return TokenT{};

		TokenT res{};

		// get the lexeme and the final state reached (if any)
		const FSMResult fsmRes = this->m_Automatan.simulate(this->m_SourceCode, FSM_MODE::MM_LONGEST_PREFIX);
		const state_t fstate = *fsmRes.finalState.begin();

		this->_check_accepted_fsm(fsmRes);
		// check whether there is a matched lexeme

		// if a lexeme is accepted, extract it
		const size_t lexemeSize = fsmRes.indecies.end;

		m_Logger.logDebug(std::format("LexemeSize: {}", lexemeSize));
		this->m_Col += lexemeSize;
		const std::string_view lexeme = this->m_SourceCode.substr(0, lexemeSize);

		// get the token
		res = m_TokenFactory(fstate, lexeme);

		// erase the lexeme from source code stream
		m_SourceCode.remove_prefix(lexemeSize);
		m_Logger.logDebug(std::format("Source Code: {}, Length {}", m_SourceCode, m_SourceCode.length()));

		return res;
	}


	template<typename TokenT, typename TableT, typename InputT>
	TokenT m0st4fa::LexicalAnalyzer<TokenT, TableT, InputT>::peak(unsigned flags) {

		// remove all white spaces and count new lines
		if (-not (flags & (unsigned)LA_FLAG::LAF_ALLOW_WHITE_SPACE_CHARS))
			this->_remove_whitespace(flags);

		// if we are at the end of the source code or it is empty, return EOF token
		if (this->_check_source_code_empty())
			return TokenT{};

		TokenT res{};

		// get the lexeme and the final state reached (if any)
		const FSMResult fsmRes = this->m_Automatan.simulate(this->m_SourceCode, FSM_MODE::MM_LONGEST_PREFIX);
		const state_t fstate = *fsmRes.finalState.begin();

		// check whether there is a matched lexeme
		this->_check_accepted_fsm(fsmRes);

		// if a lexeme is accepted, extract it
		const size_t lexemeSize = fsmRes.indecies.end;

		m_Logger.logDebug(std::format("LexemeSize: {}", lexemeSize));
		const std::string lexeme = this->m_SourceCode.substr(0, lexemeSize);

		// get the token
		res = m_TokenFactory(fstate, lexeme);

		// do not erase the lexeme from source code stream
		m_Logger.logDebug(std::format("SourceCode: {}, Length {}", m_SourceCode, m_SourceCode.length()));

		return res;
	}

};
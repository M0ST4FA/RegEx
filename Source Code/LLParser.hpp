#pragma once
// std includes
#include <vector>


// local includes
#include "Parser.h"

namespace m0st4fa {

	template <typename SymbolT, typename TokenT,
		typename ParsingTableT = LLParsingTable<>, typename FSMTableT = FSMTable<>,
		typename InputT = std::string> 
	class LLParser : public Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT> {

		// a parsing table entry should contain indecies into this vector of productions
		std::vector<ProductionRecord<SymbolT>> m_ProdRecords;
		std::vector<StackElement<SymbolT>> m_Stack;
		StackElement<SymbolT> m_CurrTopElement;
		TokenT m_CurrInputToken;

		void reset_parser_state(bool resetProductions = false) { 
			m_Stack.clear();
			m_CurrTopElement = StackElement<SymbolT>{};
			m_CurrInputToken = TokenT{};

			if (resetProductions)
				m_ProdRecords.clear();

			return;
		};

		/*
		* @brief implements error recovery.
		*/
		void error_recovery(ErrorRecoveryType errRecovType = ErrorRecoveryType::ERT_NONE) {

			// hande invalid arguments
			if (errRecovType == ErrorRecoveryType::ERT_NUM) {
				LoggerInfo info = { .level = LOG_LEVEL::LL_ERROR, .info {.errorType = ERROR_TYPE::ET_INVALID_ARGUMENT} };

				this->m_Logger.log(info, "[ERR_RECOVERY]: Invalid argument.");

				throw std::invalid_argument("Argument ERT_NONE cannot be used in this context.\nNote: it is just for knowing the number of possible values of this enum.");
			}

			this->m_Logger.logDebug("[ERR_RECOVERY]: started error recovery: " + stringfy(errRecovType));

			switch (errRecovType) {

			case ErrorRecoveryType::ERT_NONE:
				return;

			case ErrorRecoveryType::ERT_PANIC_MODE:
				panic_mode();
				return;

			case ErrorRecoveryType::ERT_PHRASE_LEVE:
				phrase_level();
				return;

			case ErrorRecoveryType::ERT_GLOBAL:
				global();
				return;

			case ErrorRecoveryType::ERT_ABORT:
				std::abort();
				break;

			default:
				// TODO
				break;
			};

		};

		/*
		* @brief implements panic mode error recovery.
		*/
		void panic_mode();

		// TODO: implement phrase level and global error recovery
		void phrase_level() { return; };
		void global() { return; };

	public:

		// constructors
		LLParser() = default;
		LLParser(
			const std::vector<ProductionRecord<SymbolT>>& prodRecords,
			const SymbolT& startSymbol,
			const ParsingTableT& parsingTable,
			const LexicalAnalyzer<TokenT, FSMTableT>& lexer
			) :
			Parser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>
			{ lexer, parsingTable, startSymbol },
			m_ProdRecords{ prodRecords },
			m_Stack{},
			m_CurrInputToken{},
			m_CurrTopElement{}
		{};

		// methods
		/**
		* @output If w is in L(G), a leftmost derivation of w; otherwise, an error indication.
		* It might execute actions during the leftmost derivation, for example, to make a parsing or syntax tree.
		*/
		ParserResult parse(ExecutionOrder, ErrorRecoveryType = ErrorRecoveryType::ERT_NONE);


	};


	// IMPLEMENTATIONS
	template<typename SymbolT, typename TokenT, typename ParsingTableT, typename FSMTableT, typename InputT>
	ParserResult LLParser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>::parse(ExecutionOrder exeuctionOrder, ErrorRecoveryType errRecoveryType)
	{
		ParserResult res;

		// Initalize the stack, such that the parser is in the initial configuration
		// push back the start symbol
		m_Stack.push_back({SET_GRAM_SYMBOL, {this->getStartSymbol()}});
		
		/**
		* Loop until the stack is empty.
		* Consider the current symbol on top of the stack and the current input.
		* Make the parsing descision based on these two tokens.
		* Whenever a symbol is matched, it is popped off the stack.
		* The purpose is to pop the start symbol off the stack (to match it, leaving the stack empty) and not produce any errors.
		*/
		this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
		while (-not m_Stack.empty()) {

			// get the current symbol on top of the stack, pop it and get the next input token
			m_CurrTopElement = m_Stack.back();
			const SymbolT topSymbol = m_CurrTopElement.as.gramSymbol;
			m_Stack.pop_back();


			// if the symbol at the top of the stack is a terminal symbol
			if (topSymbol.isTerminal) {

				if (topSymbol == TokenT::EPSILON)
					continue;
				
				// match it explicitly
				bool matched = (topSymbol == m_CurrInputToken);

				std::clog << "Matched " << topSymbol << " with " << m_CurrInputToken << ": " << std::boolalpha << matched << std::endl;
				m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();

				// if the symbol at the top of the stack is not a terminal symbol and the input token is not matched,
				if (-not matched)
					error_recovery(errRecoveryType);
				
			}
			// if the symbol is a non-terminal symbol
			else {

				// get the production record for the current symbol and input
				const TableEntry tableEntry = this->m_Table[EXTRACT_VARIABLE(this->m_CurrTopElement)][(size_t)m_CurrInputToken.name];

				// if the table entry is an error
				if (tableEntry.isError)
					error_recovery(errRecoveryType);

				// if the table entry is not an error

				// Caution: this is a reference
				const auto& prod = this->m_ProdRecords[tableEntry.prodIndex];
				const auto& prodBody = prod.prodBody;

				// if the production record is empty
				if (prodBody.empty()); // TODO: error

				// push the body of the production on top of the stack
				for (auto it = prodBody.rbegin(); it != prodBody.rend(); ++it) {
					StackElement<SymbolT> se = *it;
					m_Stack.push_back(se);
				};
				std::cout << "Expanded " << topSymbol << " with " << m_CurrInputToken << ": " << prod << std::endl;
				
			}
			
		}

		reset_parser_state();
		return res;
	}

	template<typename SymbolT, typename TokenT, typename ParsingTableT, typename FSMTableT, typename InputT>
	void LLParser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>::panic_mode()
	{
		// get top stack element
		auto currInputToken = this->m_CurrInputToken;
		const SymbolT topSymbol = m_CurrTopElement.as.gramSymbol;

		// set up the logger state
		LoggerInfo info{ .level = LOG_LEVEL::LL_INFO, .info { .noVal = 0} };

		// loop until the input and the stack are synchronized
		while (true) {

			// if the top symbol is a terminal
			if (topSymbol.isTerminal) {
				this->m_Logger.log(info, std::format(
					"Added lexeme {:s} to the input stream.", currInputToken.attribute)
				);

				// pop the token of the stack and return to the parser
				m_Stack.pop_back();
				return;
			}
			// if the top symbol is a non-terminal
			else {

				// peak to see the next token
				currInputToken = this->getLexicalAnalyzer().peak();

				// check if it can be used to sync with the parser

				// get the production record for the current symbol and input
				const TableEntry tableEntry = this->m_Table[EXTRACT_VARIABLE(this->m_CurrTopElement)][(size_t)currInputToken.name];

				/**
				* Assume the syncronization set of each non-terminal contains the first set of that non-terminal.
				* For the rest of the tokens, the action to take would be specified in the table entry.
				*/

				/**
				* If the entry is an error, check whether it has an action or no.
				* If it has an associated action, do it, otherwise, continue to skip tokens.
				*/ 
				if (tableEntry.isError) {

					if (tableEntry.action) {
						auto action = static_cast<bool (*)(Stack<SymbolT>, StackElement<SymbolT>, TokenT)>(tableEntry.action);

						// if the action results in a syncronization
						if (action(m_Stack, m_CurrTopElement, currInputToken)) {
							m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
							this->m_Logger.log(info, "Syncronized successfully.");
							return;
						}

						m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
						continue;
					}
					
					// if the entry has no action
					m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
					continue;
				}

				// get the next token
				m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
				this->m_Logger.log(info, "Syncronized successfully.");

				/**
				* If the table entry is not an error, then we have syncronized correctly using FIRST set and possibly using FOLLOW set.
				* Since the state is now syncronized, we have to return to the parser to continue parsing the source.
				*/
				
				return;
			}
		}
		
		return;
	}

}

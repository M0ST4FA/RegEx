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
		mutable std::vector<ProductionRecord<SymbolT>> m_ProdRecords;
		mutable std::vector<StackElement<SymbolT>> m_Stack;
		mutable StackElement<SymbolT> m_CurrTopElement;
		mutable TokenT m_CurrInputToken;

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
		bool error_recovery(ErrorRecoveryType errRecovType = ErrorRecoveryType::ERT_NONE) {

			// hande invalid arguments
			if (errRecovType == ErrorRecoveryType::ERT_NUM) {
				LoggerInfo info = { .level = LOG_LEVEL::LL_ERROR, .info {.errorType = ERROR_TYPE::ET_INVALID_ARGUMENT} };

				this->m_Logger.log(info, "[ERR_RECOVERY]: Invalid argument.");

				throw std::invalid_argument("Argument ERT_NONE cannot be used in this context.\nNote: it is just for knowing the number of possible values of this enum.");
			}

			this->m_Logger.logDebug("[ERR_RECOVERY]: started error recovery: " + stringfy(errRecovType));

			switch (errRecovType) {

			case ErrorRecoveryType::ERT_NONE:
				return false;

			case ErrorRecoveryType::ERT_PANIC_MODE:
				return panic_mode();

			case ErrorRecoveryType::ERT_PHRASE_LEVE:
				return phrase_level();

			case ErrorRecoveryType::ERT_GLOBAL:
				return global();

			case ErrorRecoveryType::ERT_ABORT:
				std::abort();
				break;

			default:
				// TODO
				return false;
			};

		};

		/*
		* @brief implements panic mode error recovery.
		*/
		bool panic_mode();
		bool panic_mode_try_sync_variable(TokenT&);

		// TODO: implement phrase level and global error recovery
		bool phrase_level() const { return false; };
		bool global() const { return false; };

		// error helper functions

		/**
		* Checks that the body of the given production is not empty.
		* If it is empty, the function reports the error and throws a logic_exception.
		*/
		void check_prod_body(const ProductionRecord<SymbolT>&) const;
		void print_sync_msg(const std::pair<size_t, size_t> pos) const {
			
			static constexpr LoggerInfo info = { .level = LOG_LEVEL::LL_INFO, .info = {.noVal = 0} };

			std::string msg = std::format("({}, {}) Syncronized successfully. Current input token {}",
				pos.first,
				pos.second,
				(std::string)m_CurrInputToken);

			this->m_Logger.log(info, msg);
		};

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
		LoggerInfo info{ .level = LOG_LEVEL::LL_INFO, .info {.noVal = 0} };

		// Initalize the algorithm, such that the parser is in the initial configuration
		m_Stack.push_back({ SET_GRAM_SYMBOL, {this->getStartSymbol()} });
		this->m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();

		/** Basic algorithm:
		* Loop until the stack is empty.
		* Consider the current symbol on top of the stack and the current input.
		* Make the parsing descision based on these two tokens.
		* Whenever a symbol is matched, it is popped off the stack.
		* The purpose is to pop the start symbol off the stack (to match it, leaving the stack empty) and not produce any errors.
		*/
		while (-not m_Stack.empty()) {

			// get the current symbol on top of the stack, pop it and get the next input token
			m_CurrTopElement = m_Stack.back();
			const SymbolT topSymbol = m_CurrTopElement.as.gramSymbol;
			m_Stack.pop_back();


			// if the symbol at the top of the stack is a terminal symbol
			if (topSymbol.isTerminal) {

				// epsilon matches with nothing
				if (topSymbol == TokenT::EPSILON)
					continue;

				// match it explicitly
				bool matched = (topSymbol == m_CurrInputToken);

				this->m_Logger.log(info, std::format("Matched {:s} with {:s}: {:s}", (std::string)topSymbol, (std::string)m_CurrInputToken, matched ? "true" : "false"));

				// get the next input token
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
				// TODO: check for sync state
				if (tableEntry.isError) {
					// TODO: do more robust logic based on the boolean returned from the function
					error_recovery(errRecoveryType);
					continue; // assuming we have syncronized and are ready to continue parsing
				}

				// if the table entry is not an error

				// Caution: this is a reference
				const auto& prod = this->m_ProdRecords[tableEntry.prodIndex];
				const auto& prodBody = prod.prodBody;

				// check that the production body is not empty
				check_prod_body(prod);

				// push the body of the production on top of the stack
				for (auto it = prodBody.rbegin(); it != prodBody.rend(); ++it) {
					StackElement<SymbolT> se = *it;
					m_Stack.push_back(se);
				};

				this->m_Logger.log(info, std::format("Expanded {:s} with {:s}: {:s}", (std::string)topSymbol, (std::string)m_CurrInputToken, (std::string)prod));


			}

		}

		reset_parser_state();
		return res;
	}

	template<typename SymbolT, typename TokenT, typename ParsingTableT, typename FSMTableT, typename InputT>
	bool LLParser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>::panic_mode()
	{
		// get top stack element
		auto currInputToken = this->m_CurrInputToken;
		const SymbolT topSymbol = m_CurrTopElement.as.gramSymbol;

		// set up the logger state
		LoggerInfo info{ .level = LOG_LEVEL::LL_INFO, .info {.noVal = 0} };
		LoggerInfo errInfo{ .level = LOG_LEVEL::LL_ERROR, .info {.errorType = ERROR_TYPE::ET_UNEXCPECTED_TOKEN } };

		this->m_Logger.log(errInfo, std::format(
			"({}, {}) Didn't excpect token {:s}",
			this->getLexicalAnalyzer().getLine(),  
			this->getLexicalAnalyzer().getCol(),
			(std::string)this->m_CurrInputToken
		));

		// loop until the input and the stack are synchronized
		while (true) {

			// if the top symbol is a terminal
			if (topSymbol.isTerminal) {

				
				this->m_Logger.log(info, std::format(
					"Added lexeme {:s} to the input stream.", currInputToken.attribute)
				);

				// pop the token of the stack and return to the parser
				m_Stack.pop_back();
				return true;
			}
			// if the top symbol is a non-terminal
			else {

				// peak to see the next token
				currInputToken = this->getLexicalAnalyzer().peak();

				// check if it can be used to sync with the parser
				bool synced = this->panic_mode_try_sync_variable(currInputToken);
				
				if (synced)
					break;

				// if we've reached the end of the input and could not sync with this token
				if (m_CurrInputToken == TokenT{}) {
					// pop this element since there is no way to syncronize using it
					m_Stack.pop_back();

					// if the stack is empty, return to the caller
					if (m_Stack.empty()) {
						this->m_Logger.log(info, std::format("[ERROR_RECOVERY] ({}, {}) Failed to syncronize: current input: {:s}",
							this->getLexicalAnalyzer().getLine(),
							this->getLexicalAnalyzer().getCol(),
							(std::string)currInputToken
						));

						return false;
					};
					
				};

				
			}
		}

		return true;
	}

	/** Actions:
	* If the non-terminal has an epsilon production, make it the default.
	* If there is a token whose associated table entry has an error action, execute it.
	* If we find a token that is in the first set of the currunt token, expand by it.
	*/
	template<typename SymbolT, typename TokenT, typename ParsingTableT, typename FSMTableT, typename InputT>
	bool LLParser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>::panic_mode_try_sync_variable(TokenT& currInputToken) {

		LoggerInfo info{ .level = LOG_LEVEL::LL_INFO, .info {.noVal = 0} };

		// Check for whether the non-terminal has an epsilon production and use it to reduce that non-terminal.
		TableEntry tableEntry = this->m_Table[EXTRACT_VARIABLE(this->m_CurrTopElement)][(size_t)TokenT::EPSILON.name];

		// if it is not an err, there is an epsilon production
		if (-not tableEntry.isError) {
			// expand with this production and assume we are in sync

			// Caution: this is a reference
			const auto& prod = this->m_ProdRecords[tableEntry.prodIndex];
			const auto& prodBody = prod.prodBody;

			// check that the production body is not empty
			check_prod_body(prod);


			// push the body of the production on top of the stack
			for (auto it = prodBody.rbegin(); it != prodBody.rend(); ++it) {
				StackElement<SymbolT> se = *it;
				m_Stack.push_back(se);
			};

			this->m_Logger.log(info, std::format("[ERROR_RECOVERY] Expanded {:s} with {:s}: {:s}",
				stringfy(m_CurrTopElement.as.gramSymbol.as.nonTerminal),
				m_CurrInputToken.toString(),
				prod.toString()));

			print_sync_msg(this->getLexicalAnalyzer().getPosition());

			return true;
		}
		
		// get the production record for the current symbol and input
		tableEntry = this->m_Table[EXTRACT_VARIABLE(this->m_CurrTopElement)][(size_t)currInputToken.name];

		/**
		* Assume the syncronization set of each non-terminal contains the first set of that non-terminal.
		* For the rest of the tokens, the action to take would be specified in the table entry.
		*/

		/**
		* If the entry is an error, check whether it has an action or no.
		* If it has an associated action, do it, otherwise, continue to skip tokens.
		*/
		if (tableEntry.isError) {

			// if the entry has an action
			if (tableEntry.action) {
				auto action = static_cast<bool (*)(Stack<SymbolT>, StackElement<SymbolT>, TokenT)>(tableEntry.action);

				// if the action results in a syncronization
				if (action(m_Stack, m_CurrTopElement, currInputToken)) {
					m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
					
					print_sync_msg(this->getLexicalAnalyzer().getPosition());
					
					return true;
				}

			}

			// if the entry has no action or the action has failed to syncronize

			// get the next input and try again to syncronize
			m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
			return false;
		}

		//---------------------------------------------Syncronized-------------------------------------------
		/**
		* Upon reaching here, this means the parser has synronized with the current token.
		* Since we've just peaked to see whether we can sync with it or not, now we need to fetch it so that parsing can continue from it.
		*/
		m_CurrInputToken = this->getLexicalAnalyzer().getNextToken();
		print_sync_msg(this->getLexicalAnalyzer().getPosition());

		/**
		* If the table entry is not an error, then we have syncronized correctly using FIRST set and possibly using FOLLOW set.
		* Since the state is now syncronized, we have to return to the parser to continue parsing the source.
		*/

		return true;
	}

	template<typename SymbolT, typename TokenT, typename ParsingTableT, typename FSMTableT, typename InputT>
	void inline LLParser<SymbolT, TokenT, ParsingTableT, FSMTableT, InputT>::check_prod_body(const ProductionRecord<SymbolT>& prod) const {

		LoggerInfo errorInfo { .level = LOG_LEVEL::LL_ERROR, .info {.errorType = ERROR_TYPE::ET_PROD_BODY_EMPTY } };

		const auto& prodBody = prod.prodBody;

		if (prodBody.empty()) {
			this->m_Logger.log(errorInfo, std::format("Production body is empty: {:s}", prod.toString()));
			throw std::logic_error("Production body is empty.");
		};

	}

}

#pragma once
#pragma once

#include <iomanip>

#include "PDataStructs.h"

// DECLARATION
namespace m0st4fa {
	template<typename GrammarT, typename LLParsingTableT>
		requires requires (GrammarT grammar) {
		true;
		/*grammar.calculateFIRT();
		grammar.getFIRST();
		grammar.calculateFOLLOW();
		grammar.getFOLLOW();*/
	}
	class LLParserGenerator {
		using ProdVecType = decltype(GrammarT{}.getProdVector());
		using ProductionType = decltype(ProdVecType{}.at(0));
		using SymbolType = decltype(ProductionType{}.prodHead);
		using VariableType = decltype(SymbolType{}.as.nonTerminal);
		using TerminalType = decltype(SymbolType{}.as.terminal);
		using SetType = decltype(GrammarT{}.getFOLLOW(SymbolType{}.as.nonTerminal));

		// Assume FIRST set is stored within the grammar class
		SymbolType m_StartSymbol{};
		mutable LLParsingTableT m_ParsingTable{};
		GrammarT& m_Grammar{};
		mutable bool m_ParsingTableGenerated = false;

		void _fill_table(const ProductionType&, size_t) const;

	protected:
		Logger p_Logger{};

	public:
		LLParserGenerator() = default;
		LLParserGenerator(const LLParserGenerator&) = default;
		LLParserGenerator(LLParserGenerator&&) = default;
		LLParserGenerator(const GrammarT& grammar, const SymbolType& startSymbol) : m_ParsingTable { grammar }, m_Grammar {this->m_ParsingTable.grammar}, m_StartSymbol{ startSymbol } {
			this->m_ParsingTable.grammar.calculateFIRST();
			this->m_ParsingTable.grammar.calculateFOLLOW();
		};

		SymbolType getStartSymbol() { return this->m_StartSymbol; }
		const LLParsingTableT& generateLLParser() const;
		const GrammarT& getGrammar() {
			return this->m_Grammar;
		}

	};
}

// IMPLEMENTATION
namespace m0st4fa {

	template<typename GrammarT, typename LLParsingTableT>
		requires requires (GrammarT grammar) {
	true;
	/*grammar.calculateFIRT();
	grammar.getFIRST();
	grammar.calculateFOLLOW();
	grammar.getFOLLOW();*/
	}
	void LLParserGenerator<GrammarT, LLParsingTableT>::_fill_table(const ProductionType& production, size_t prodIndex) const 
	{

		const SymbolType& head = production.prodHead;
		size_t headIndex = (size_t)head.as.nonTerminal;
		SetType terminalSet{};

		// if this is an EPSILON-production
		if (production.isEpsilon())
			terminalSet = this->m_Grammar.getFOLLOW(head.as.nonTerminal);
		// if this is not an EPSILON-production
		else {
			SymbolString<SymbolType> string = production.toSymbolString();
			string.calculateFIRST(this->m_Grammar.getFIRST());
			terminalSet = string.getFIRST();
		}

		// loop through every terminal in FIRST(H)/FOLLOW(H)
		for (const SymbolType& terminal : terminalSet) {
			size_t terminalIndex = (size_t)terminal.as.terminal;
			LLTableEntry& currEntry = this->m_ParsingTable[headIndex][terminalIndex];

			this->p_Logger.log(LoggerInfo::DEBUG, std::format("Defining LL(1) parsing table entry indexed: [{}][{}]", stringfy(head.as.nonTerminal), stringfy(terminal.as.terminal)));

			// check if the current entry is already occupied
			if (not currEntry.isEmpty) {

				this->p_Logger.log(LoggerInfo::ERR_INVALID_VAL, std::format("Redefinition of LL(1) parsing table entry indexed: [{}][{}]", stringfy(head.as.nonTerminal), stringfy(terminal.as.terminal)));
				throw std::logic_error("Redefinition of entry of LL(1) parsing table. This grammar is not LL(1).");
			}

			// the current entry is not already occupied
			currEntry.isEmpty = false;
			currEntry.isError = false;
			currEntry.prodIndex = prodIndex;
		}
	}

	template<typename GrammarT, typename LLParsingTableT>
		requires requires (GrammarT grammar) {
		true;
		/*grammar.calculateFIRT();
		grammar.getFIRST();*/
		/*grammar.calculateFOLLOW();
		grammar.getFOLLOW();*/
	}
	const LLParsingTableT& LLParserGenerator<GrammarT, LLParsingTableT>::generateLLParser() const
	{

		// check whether the parsing table is already created
		if (this->m_ParsingTableGenerated) {
			this->p_Logger.logDebug("The parsing table is already created!");
			return this->m_ParsingTable;
		}

		/** Algorithm
		* For every production P and its head H:
			* If P is of the form P -> EPSILON:
				* For every terminal T in FOLLOW(H):
					* If TABLE[H][T] is already occupied: ERROR
					* Else TABLE[H][T] = P
			* If P is not an EPSILON-production:
				* For every terminal T in FIRST(H):
					* If TABLE[H][T] is already occupied: ERROR
					* Else TABLE[H][T] = P
		*/

		this->p_Logger.logDebug("CREATING LL(1) PARSING TABLE.");
		for (size_t prodIndex = 0; prodIndex < this->m_Grammar.size(); prodIndex++) {
			const ProductionType& production = this->m_Grammar.at(prodIndex);
			_fill_table(production, prodIndex);
		}
		this->p_Logger.logDebug("LL(1) PARSING TABLE CREATED:");
		size_t startVarIndex = (size_t)this->m_StartSymbol.as.nonTerminal;

#if defined(_DEBUG)
#define COLUMN_WIDTH 12
#define COLUMNS_WIDTH (COLUMN_WIDTH * (size_t)TerminalType::T_COUNT + (size_t)TerminalType::T_COUNT)

		std::clog << std::setfill('-') << std::setw(COLUMNS_WIDTH - 1) << '-' << std::setfill(' ') << '\n';
		std::clog << "|" << "VAR/TERM" << std::setw(COLUMN_WIDTH - strlen("VAR/TERM") - 1) << "|";
		// loop through every terminal of the grammar
		for (size_t t = 0; t < (size_t)TerminalType::T_COUNT; t++) {
			TerminalType terminal = (TerminalType)t;
			size_t remaining = COLUMN_WIDTH - stringfy(terminal).size() - 1;
			std::string temp{}; 
			temp.resize(remaining / 2 > 0 ? remaining / 2 : 0, ' ');
			std::clog << temp << terminal << std::setw(temp.size()) << "|";
		}
		std::clog <<'\n' << std::setfill('-') << std::setw(COLUMNS_WIDTH - 1) << '-' << std::setfill(' ') << "\n";

		// loop through every variable of the grammar starting at the start symbol
		for (size_t variable = startVarIndex; variable < (size_t)VariableType::NT_COUNT; variable++) 
		{

			VariableType nonTerminal = (VariableType)variable;
			std::string varStr = stringfy(nonTerminal);
			size_t remaining = COLUMN_WIDTH - varStr.size() - 1;
			std::string temp{};
			temp.resize(remaining / 2 > 0 ? remaining / 2 : 0, ' ');
			std::clog << '|' << temp << varStr << std::setw(temp.size() % 2 ? temp.size() : temp.size() + 1) << "|";

			for (size_t t = 0; t < (size_t)TerminalType::T_COUNT; t++) {
				LLTableEntry& entry = this->m_ParsingTable[variable][t];

				if (entry.isEmpty) {
					std::clog << std::setw(COLUMN_WIDTH - 2) << '|';
					continue;
				}
				
				std::string prodIndex = std::to_string(entry.prodIndex);
				size_t remaining = COLUMN_WIDTH - prodIndex.size() - 1;
				std::string temp{};
				temp.resize(remaining / 2 > 0 ? remaining / 2 : 0, ' ');
				std::clog << temp << prodIndex << std::setw(temp.size()) << "|";

			}

			std::clog << std::endl;
		}

		std::clog << std::setfill('-') << std::setw(COLUMNS_WIDTH - 1) << '-' << std::setfill(' ') << '\n';

		std::cout << "GRAMMAR:\n" << this->m_Grammar.toString() << "\n";
#undef COLUMN_WIDTH
#endif

		this->m_ParsingTableGenerated = true;
		return this->m_ParsingTable;
	}

}

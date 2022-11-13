#pragma once
#include <array>
#include <iostream>
#include <format>

#include "PStack.h"
#include "PProduction.h"

// LL PARSER
namespace m0st4fa {

	struct LLTableEntry {
		bool isError = true;

		union {
			size_t prodIndex = SIZE_MAX;
			void* action;
		};

		bool isEmpty = true;

		std::string toString() {

			std::string msg = isError ? "Error Entry\n" : std::format("Production Index of table entry: {}\n", prodIndex);

			return msg;
		};
	};

	template <typename GrammarT, size_t VariableCount = 20, size_t TerminalCount = 127>
	struct LLParsingTable {
		GrammarT grammar;
		std::array<std::array<LLTableEntry, TerminalCount>, VariableCount> table;

		std::array<LLTableEntry, TerminalCount>& operator[](size_t index) {
			return table[index];
		}

	};
}

// LR PARSER
namespace m0st4fa {

	// LR TABLE ENTRY
	enum class LRTableEntryType
	{
		TET_ACTION_SHIFT,
		TET_ACTION_REDUCE,
		TET_GOTO,
		TET_ACCEPT,
		TET_ERROR,
		TET_COUNT
	};
	std::string stringfy(const LRTableEntryType);

	struct LRTableEntry {
		bool isEmpty = true;
		LRTableEntryType type = LRTableEntryType::TET_ERROR;
		size_t number = SIZE_MAX;

		operator std::string() const {
			return this->toString();
		}
		std::string toString() const {

			std::string msg;

			switch (type) {
			case LRTableEntryType::TET_ACTION_SHIFT:
				msg += std::format("SHIFT {}", number);
				break;
			case LRTableEntryType::TET_ACTION_REDUCE:
				msg += std::format("REDUCE {}", number);
				break;
			case LRTableEntryType::TET_GOTO:
				msg += std::format("{}", number);
				break;
			
			default: // ERROR & EMTPY produce an empty string
				return msg;
			}

			return msg;
		}

		// Note: being empty is considered erranuous
		inline bool isError() const {
			return isEmpty || this->type == LRTableEntryType::TET_ERROR;
		}

		inline bool isAccept() const {
			return this->type == LRTableEntryType::TET_ACCEPT;
		}
	};

	// TABLE ENTRY MACROS
#define TE_SHIFT(state) m0st4fa::LRTableEntry{false, m0st4fa::LRTableEntryType::TET_ACTION_SHIFT, state}
#define TE_REDUCE(prodNum) m0st4fa::LRTableEntry{false, m0st4fa::LRTableEntryType::TET_ACTION_REDUCE, prodNum}
#define TE_GOTO(state) m0st4fa::LRTableEntry{false, m0st4fa::LRTableEntryType::TET_GOTO, state}
#define TE_ACCEPT() m0st4fa::LRTableEntry{false, m0st4fa::LRTableEntryType::TET_ACCEPT}
#define TE_ERROR() m0st4fa::LRTableEntry{false, m0st4fa::LRTableEntryType::TET_ERROR}

	// LR PARSING TABLE
	template <typename GrammarT>
	struct LRParsingTable {
		using ProductionType = decltype(GrammarT{}.at(0));
		using SymbolType = decltype(ProductionType{}.prodHead);
		using VariableType = decltype(SymbolType{}.as.nonTerminal);
		using TerminalType = decltype(SymbolType{}.as.terminal);

		static constexpr const size_t VariableCount = (size_t)VariableType::NT_COUNT;
		static constexpr const size_t TerminalCount = (size_t)TerminalType::T_COUNT;
		using ActionArrayType = std::array<LRTableEntry, TerminalCount>;
		using GotoArrayType = std::array<LRTableEntry, VariableCount>;

		GrammarT grammar;
		std::vector<ActionArrayType> actionTable;
		std::vector<GotoArrayType> gotoTable;

		LRParsingTable() = default;
		LRParsingTable(const GrammarT& grammar,
			const std::vector<ActionArrayType>& actionTable,
			const std::vector<GotoArrayType>& gotoTable)
			: grammar{ grammar }, actionTable{ actionTable }, gotoTable{ gotoTable }
		{}

		LRTableEntry& at(size_t state, SymbolType symbol) {
			if (symbol.isTerminal)
				return atAction(state, symbol.as.terminal);

			return atGoto(state, symbol.as.nonTerminal);
		}

		LRTableEntry& atAction(size_t state, TerminalType terminal) {
			try {
				return this->actionTable[state][(size_t)terminal];
			}
			catch(...) {
				assert("TODO: handle checking the boundry of action table");
				this->reserveRows(state);
			}

			return this->actionTable[state][(size_t)terminal];
		};
		const ActionArrayType& atActionRow(size_t state) const {
			return this->actionTable.at(state);
		}
		/**
		* Return all non-error action entries for this state
		*/
		std::vector<TerminalType> getActions(size_t state) const {
			std::vector<TerminalType> res;

			for (size_t terminal = 0; terminal < TerminalCount; terminal++)
				if (not this->actionTable[state][terminal].isError())
					res.push_back((TerminalType)terminal);

			return res;
		}

		LRTableEntry& atGoto(size_t state, VariableType nonTerminal) {
			try {
				return this->gotoTable[state][(size_t)nonTerminal];
			}
			catch(...) {
				assert("TODO: handle checking the boundry of goto table");
				this->reserveRows(state);
			}

			return this->gotoTable[state][(size_t)nonTerminal];
		};
		const GotoArrayType& atGotoRow(size_t state) const {
			return this->gotoTable.at(state);
		}
		/**
		* Return all non-error goto entries for this state
		*/
		std::vector<VariableType> getGotos(size_t state) const {
			std::vector<VariableType> res;

			for (size_t variable = 0; variable < VariableCount; variable++)
				if (not this->gotoTable[state][variable].isError())
					res.push_back((VariableType)variable);

			return res;
		}

		void reserveRows(size_t newRowNum) {
			this->actionTable.resize(newRowNum);
			this->gotoTable.resize(newRowNum);
		}

	};

};
#pragma once
#include <array>
#include <iostream>
#include <format>

#include "PStack.h"
#include "PProduction.h"

namespace m0st4fa {

	// LL TABLE

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


	// LR TABLE

	enum class LRTableEntryType
	{
		TET_ACTION_SHIFT,
		TET_ACTION_REDUCE,
		TET_GOTO,
		TET_ERROR,
		TET_COUNT
	};

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

		inline bool isError() const {
			return this->type == LRTableEntryType::TET_ERROR;
		}
	};

	template <typename GrammarT, typename VariableT, typename TerminalT>
	struct LRParsingTable {
		static constexpr const size_t VariableCount = (size_t)VariableT::NT_COUNT;
		static constexpr const size_t TerminalCount = (size_t)TerminalT::T_COUNT;
		using ActionArrayType = std::array<LRTableEntry, TerminalCount>;
		using GotoArrayType = std::array<LRTableEntry, VariableCount>;

		GrammarT grammar;
		std::vector<ActionArrayType> actionTable;
		std::vector<GotoArrayType> gotoTable;

		LRTableEntry& atAction(size_t state, TerminalT terminal) {
			return this->actionTable[state][(size_t)terminal];
		};
		LRTableEntry& atGoto(size_t state, VariableT nonTerminal) {
			return this->gotoTable[state][(size_t)nonTerminal];
		};

		void reserveRows(size_t newRowNum) {
			this->actionTable.reserve(newRowNum);
			this->gotoTable.reserve(newRowNum);
		}

	};

};
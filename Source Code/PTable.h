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

		template<typename ProductionT>
		std::ostream& toString(ProdVec<ProductionT>& prodVec) {

			std::string msg = isError ? "Error Entry\n" : std::format("Production Index of table entry: {}\n", prodIndex);

			return std::cout;
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

	struct LRTableEntry;

	struct LRParsingTable;

};
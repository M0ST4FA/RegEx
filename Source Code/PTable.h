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
			size_t prodIndex = 0;
			void* action;
		};

		template<typename SymbolT, typename SynthesizedT, typename ActionT>
		std::ostream& toString(ProdVec<SymbolT, SynthesizedT, ActionT>& prodVec) {

			std::string msg = isError ? "Error Entry\n" : std::format("Production Index of table entry: {}\n", prodIndex);

			return std::cout;
		};
	};

	template <size_t VariableCount = 20, size_t TerminalCount = 127>
	struct LLParsingTable {
		std::array<std::array<LLTableEntry, TerminalCount>, VariableCount> table;

		std::array<LLTableEntry, TerminalCount>& operator[](size_t index) {
			return table[index];
		}

	};


	// LR TABLE

	struct LRTableEntry;

	struct LRParsingTable;

};
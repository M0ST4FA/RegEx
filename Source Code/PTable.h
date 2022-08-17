#pragma once
#include <array>
#include <iostream>
#include <format>


namespace m0st4fa {

	// type aliases
	template<typename SymbolT>
	using Stack = std::vector<StackElement<SymbolT>>;

	template<typename SymbolT>
	using ProdVec = std::vector<ProductionRecord<SymbolT>>;

	struct TableEntry {
		bool isError = true;

		union {
			size_t prodIndex = 0;
			void* action;
		};

		template<typename SymbolT>
		std::ostream& toString(ProdVec<SymbolT>& prodVec) {

			std::string msg = isError ? "Error Entry\n" : std::format("Production Index of table entry: {}\n", prodIndex);

			return std::cout;
		};
	};

	template <size_t VariableCount = 20, size_t TerminalCount = 127>
	struct LLParsingTable {
		std::array<std::array<TableEntry, TerminalCount>, VariableCount> table;

		std::array<TableEntry, TerminalCount>& operator[](size_t index) {
			return table[index];
		}

	};

};
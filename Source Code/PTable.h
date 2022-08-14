#pragma once
#include <array>
#include <iostream>
#include <format>


namespace m0st4fa {

	struct TableEntry {
		bool isError = true;
		size_t prodIndex = 0;

		std::ostream& print() {

			std::string msg = isError ? " \n" : std::format("Production Index of table entry: {}\n", prodIndex);

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
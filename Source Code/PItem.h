#pragma once
#include <iostream>
#include <string>
#include <set>

#include "PProduction.h"

// DECLARATOIN
namespace m0st4fa {

	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	class Item {
		using ProdRec = ProductionRecord<SymbolT, SynthesizedT, ActionT>;
		using LookAheadSet = std::set<SymbolT>;
		using pos_t = size_t;
		Logger m_Logger{};

	public:
		// Note: when implementing items, take care of non-symbol objects within the production.
		ProdRec production;
		/** Dot position rules:
		* 0 => the dot is at the beginning of the body, just before the first symbol.
			* This also means the dot is before the 0th element.
		* n, n > 0 && n < prodSize => the dot is before the nth element.
		* prodSize => the dot is at the end of the production, after the last symbol.
		* The dot position does no take non-grammar-symbol elements in mind.
		*/
		pos_t dotPos = 0;
		LookAheadSet lookaheads{};

		Item() = default;
		Item(const ProdRec& production, pos_t dotPosition, const LookAheadSet& lookaheads = {}) :
			production{ production }, dotPos{ dotPosition }, lookaheads{ lookaheads }
		{
			const auto& prodBody = this->production.prodBody;
			size_t psize = std::count_if(
				prodBody.begin(), prodBody.end(), 
				[](const auto& element) {return element.type == StackElementType::SET_GRAM_SYMBOL; }
			);

			// check that the dot position is within range
			if ((this->dotPos > psize) or (this->dotPos < 0)) {
				this->m_Logger.log(LoggerInfo::ERR_INVALID_VAL, "Invalid dot position in item. Make sure the dot position is smaller than the size of the production body.");
				throw std::logic_error("Invalid dot position in item.");
			}

		};

		bool operator==(const Item& other) const {
			// test the dot position first since it is a common discrepancy and its test is faster.
			return other.dotPos == this->dotPos && other.production == this->production;
		};

		operator std::string() const {
			return this->toString();
		};

		std::string toString() const {
			// CAUTION: be cautious of non-grammar-symbol objects!!!
			const auto& prodBody = this->production.prodBody;
			size_t psize = std::count_if(
				prodBody.begin(), prodBody.end(),
				[](const auto& element) {return element.type == StackElementType::SET_GRAM_SYMBOL; }
			);
			std::string msg = "["+ this->production.prodHead.toString() + " ->";

			for (size_t i = 0; const auto& element : this->production.prodBody) {

				// skip non-grammar-symbol objects
				if (element.type != StackElementType::SET_GRAM_SYMBOL) {
					msg += " " + (std::string)element;
					continue;
				}

				// the element is a grammar symbol
				const SymbolT& symbol = element.as.gramSymbol;

				// if the index of the symbol is the same as the dot position
				if (i == this->dotPos) {
					// this means that the dot precedes that symbol
					msg += " . " + (std::string)symbol;
					i++;
					continue;
				}

				// if the dot is not at this symbol
				msg += " " + (std::string)symbol;

				// check if it is at the end
				if (i == psize - 1)
					msg += " .";

				i++;
			}

			// if this item has any lookaheads
			if (this->lookaheads.size() > 0) {
				msg += ", " + (std::string)*this->lookaheads.begin();

				for (const SymbolT& lookahead : this->lookaheads) {
					// if this is the first lookahead, return since we have already included it
					if (lookahead == *this->lookaheads.begin())
						continue;

					// if this is not the first lookahead
					msg += " / " + (std::string)lookahead;
				}
			}

			return msg + "]";
		}

	};

}

// IMPLEMENTATION
namespace m0st4fa {



}
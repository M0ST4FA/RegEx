#pragma once
#include <iostream>
#include <string>
#include <set>
#include <vector>

#include "PProduction.h"
#include "common.h"

// DECLARATOIN
namespace m0st4fa {

	template <typename ProductionT>
	class Item {
		using SymbolT = decltype(ProductionT{}.prodHead);
		using LookAheadSet = std::set<SymbolT>;
		using pos_t = size_t;
		Logger m_Logger{};

	public:

	 	static const Item EMPTY_ITEM;

		// Note: when implementing items, take care of non-symbol objects within the production.
		ProductionT production;
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
		Item(const ProductionT& production, pos_t dotPosition, const LookAheadSet& lookaheads = {}) :
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

		/**
		* @return true if both items have the same first component, and identical lookahead sets.
		*/
		bool operator==(const Item& other) const {
			// test the dot position first since it is a common discrepancy and its test is faster.
			return (other.dotPos == this->dotPos) && (other.production == this->production) && (other.lookaheads == this->lookaheads);
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

	template <typename ProductionT>
	const Item<ProductionT>
	Item<ProductionT>::EMPTY_ITEM {};

	template <typename ItemT> 
	class ItemSet {

		using ProductionType = decltype(ItemT{}.production);
		using SymbolType = decltype(ItemT{}.production.prodHead);

		friend Item<ProductionType>;

		// stores the item set
		std::vector<ItemT> m_Set {};
		// chaches the closure of this item set
		std::vector<ItemT> m_Closure{};
	protected:
		Logger p_Logger;

		std::vector<ItemT>::iterator get_item_it(const ProductionType& production, size_t dotPosition) {
			auto begin = this->m_Set.begin();
			auto end = this->m_Set.end();

			auto predicate = [this, &production, &dotPosition](const ItemT& item) {
				const auto& itemProd = item.production;
				size_t itemDotPos = item.dotPos;

				return (itemDotPos == dotPosition) && (itemProd == production);
			};

			const auto& it = std::find_if(begin, end, predicate);

			// return an item if it is found, else return an empty item
			return it;
		}

	public:

		ItemSet() = default;
		ItemSet(const std::initializer_list<ItemT>& items) : m_Set{ items } {};

		// CAUTION: when implementing these functions, be cautiuos that a single
		// item object may represent different items.

		// OPERATOR FUNCTIONS
		/**
		* @returns true iff the two Item sets are identical.
		*/
		bool operator==(const ItemSet& rhs) const {
			return this->m_Set == rhs.m_Set;
		};
		operator std::string() const {
			return this->toString();
		}

		// NON-OPERATOR FUNCTIONS
		std::string toString() const {

			std::string str;

			for (const auto& item : this->m_Set)
				str += (std::string)item + "\n";

			return str;
		};

		/**
		* insert a new item into the item set.
		* if an item with the first compoenent is already present, it just inserts the lookahead.
		* @return whether a new item was inserted.
		*/
		bool insert(const ItemT&);
		/**
		* get all the items with this first component in this item set, if any.
		* @return emtpy Item if none is found, else a nonempty Item.
		*/
		ItemT get(const ProductionType&, size_t) const;
		/**
		* check whether this item exists in this item set.
		* `exists` means the first component is the same, and the second is a subset.
		* @return true if such an Item is found, otherwise false.
		*/
		bool contains(const ItemT&) const;
		/**
		* @return the CLOSURE set of this item set.
		*/
		ItemSet CLOSURE(const SymbolType&);
		/**
		* @return the GOTO set of this item set on a particular symbol.
		*/
		ItemSet GOTO(const SymbolType&) const;

	};
}

// IMPLEMENTATION
namespace m0st4fa {

	template<typename ItemT>
	inline bool ItemSet<ItemT>::contains(const ItemT& item) const
	{
		auto begin = this->m_Set.begin();
		auto end = this->m_Set.end();

		auto predicate = [this, &item](const ItemT& i) {
			// 'item' is considered found if we found an 'i' such that:
			// 1. the first components are the same.
			// 2. the second compoenent of 'item' is a subset of its 'i's counterpart.

			// check for equality of the first components
			if (item.dotPos != i.dotPos && item.production != i.production)
				return false;

			// check if item.lookaheads is a subset of i.lookaheads
			// i.e. if i.lookaheads includes item.lookaheads

			const auto ibegin = i.lookaheads.begin();
			const auto iend = i.lookaheads.end();
			const auto itembegin = item.lookaheads.begin();
			const auto itemend = item.lookaheads.end();

			return std::includes(ibegin, iend, itembegin, itemend);
		};

		return (std::find_if(begin, end, predicate) != end);
	}

	template<typename ItemT>
	inline ItemT ItemSet<ItemT>::get(const ProductionType& production, size_t dotPosition) const
	{
		auto begin = this->m_Set.begin();
		auto end = this->m_Set.end();

		auto predicate = [this, &production, &dotPosition](const ItemT& item) {
			const auto& itemProd = item.production;
			size_t itemDotPos = item.dotPos;

			return (itemDotPos == dotPosition) && (itemProd == production);
		};

		const auto& it = std::find_if(begin, end, predicate);

		// return an item if it is found, else return an empty item
		return (it == end ? ItemT{} : *it);
	}

	template<typename ItemT>
	bool ItemSet<ItemT>::insert(const ItemT& item)
	{
		// find an item with the same first component
		const auto it = this->get_item_it(item.production, item.dotPos);
		const auto end = this->m_Set.end();
		bool found = it != end;

		// if an entry with the same first component is found
		if (found)
			return insertAndAssert(item.lookaheads, it->lookaheads);

		// if no entry with the same first component is found
		this->m_Set.push_back(item);

		return true;
	}

}
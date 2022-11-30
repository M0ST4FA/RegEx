#pragma once
#include <iostream>
#include <string>
#include <set>
#include <vector>

#include "PProduction.h"
#include "common.h"

// DECLARATOIN
namespace m0st4fa {
	template <typename SymbolT>
	using LookAheadSet = std::set<SymbolT>;

	template <typename ProductionT>
	class Item {
		using ProductionElementType = decltype(ProductionT{}.at(0));
		using SymbolT = decltype(ProductionT{}.prodHead);
		using LookAheadSet = LookAheadSet<SymbolT>;
		using pos_t = size_t;
		Logger p_Logger{};
		pos_t m_ActualDotPos = 0;

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
				[](const auto& element) {return element.type == ProdElementType::PET_GRAM_SYMBOL; }
			);

			// check that the dot position is within range
			if ((this->dotPos > psize) or (this->dotPos < 0)) {
				this->p_Logger.log(LoggerInfo::ERR_INVALID_VAL, "Invalid dot position in item. Make sure the dot position is smaller than the size of the production body.");
				throw std::logic_error("Invalid dot position in item.");
			}

			pos_t encountered = 0;
			while (encountered != this->dotPos) {

				bool isSymbol = this->production.prodBody.at(this->m_ActualDotPos).type == ProdElementType::PET_GRAM_SYMBOL;

				if (isSymbol)
					encountered++;

				this->m_ActualDotPos++;
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
			size_t psize = this->production.size();
			bool dotEmitted = false;
			std::string msg = "["+ this->production.prodHead.toString() + " ->";

			for (size_t i = 0; const auto& element : this->production.prodBody) {

				// skip non-grammar-symbol objects
				if (element.type != ProdElementType::PET_GRAM_SYMBOL) {
					msg += " " + (std::string)element;
					continue;
				}

				// the element is a grammar symbol
				const SymbolT& symbol = element.as.gramSymbol;

				// if the index of the symbol is the same as the dot position
				if (i == this->dotPos) {
					// this means that the dot precedes that symbol
					msg += " . " + (std::string)symbol;
					dotEmitted = true;
					i++;
					continue;
				}

				// if the dot is not at this symbol
				msg += " " + (std::string)symbol;

				// check if it is at the end
				if (i == psize - 1 && not dotEmitted)
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

		size_t getActualDotPosition() const { return this->m_ActualDotPos; }
		bool isDotPositionAtEnd() const { return this->m_ActualDotPos == this->production.size(); }

		/**
		* @output returns the production element at the dot position, or otherwise a default-constructed production element if the dot is at the end of the production.
		*/
		ProductionElementType atDotPosition() const { 
			if (this->m_ActualDotPos < this->production.size())
				return this->production.at(this->m_ActualDotPos);

			return ProductionElementType{};
		}

		/**
		* @output returns the symbol at the dot position, or otherwise a default-constructed symbol if the dot is at the end of the production.
		*/
		SymbolT symbolAtDotPosition() const { 
			if (this->m_ActualDotPos < this->production.size())
				return this->production.at(this->m_ActualDotPos).as.gramSymbol; 

			return SymbolT{};
		}
		
	};

	template <typename ProductionT>
	const Item<ProductionT>
	Item<ProductionT>::EMPTY_ITEM {};

	template <typename ItemT> 
	class ItemSet {

		using ProductionType = decltype(ItemT{}.production);
		using StackElemType = decltype(ProductionType{}.at(0));
		using SymbolType = decltype(ProductionType{}.prodHead);
		using VariableType = decltype(SymbolType{}.as.nonTerminal);
		using SymbolStringType = SymbolString<SymbolType>;
		using ProdVecType = ProductionVector<ProductionType>;
		using LookAheadSet = LookAheadSet<SymbolType>;

		friend Item<ProductionType>;

		// stores the item set
		std::vector<ItemT> m_Set {};
		// caches the closure of this item set
		std::vector<ItemT> m_Closure{};

		bool _add_to_closure_no_lookaheads(const ProdVecType&);
		bool _add_to_closure_lookaheads(const ProdVecType&, const LookAheadSet&);
		void _set_closure(const ItemSet& newClosure) {
			this->m_Closure = newClosure;
		}

	protected:
		Logger p_Logger;

		std::vector<ItemT>::iterator get_item_it(const ProductionType& production, size_t dotPosition, bool fromm_Set = true) {
			auto begin = fromm_Set ? this->m_Set.begin() : this->m_Closure.begin();
			auto end = fromm_Set ? this->m_Set.end() : this->m_Closure.end();

			auto predicate = [this, &production, &dotPosition](const ItemT& item) {
				const auto& itemProd = item.production;
				size_t itemDotPos = item.dotPos;

				return (itemDotPos == dotPosition) && (itemProd == production);
			};

			const auto& it = std::find_if(begin, end, predicate);

			// return an item if it is found, else return an empty item
			return it;
		}

		ItemSet(const std::vector<ItemT> items, bool sameClosure) : m_Set{ items }, m_Closure{ items } {}
	public:

		ItemSet() = default;
		ItemSet(const std::initializer_list<ItemT>& items) : m_Set{ items } {};
		ItemSet(const std::vector<ItemT> items) : m_Set{ items } {};
		ItemSet(const ItemSet& other) : m_Set{ other.m_Set }, m_Closure{ other.m_Closure } {};
		ItemSet(ItemSet&& other) : m_Set{ std::move(other.m_Set) }, m_Closure{ std::move(other.m_Closure) } {};

		// CAUTION: when implementing these functions, be cautiuos that a single
		// item object may represent different items.

		// OPERATOR FUNCTIONS
		/**
		* @returns true iff the two Item sets are identical.
		*/
		bool operator==(const ItemSet& rhs) const {
			return this->m_Set == rhs.m_Set;
		};
		bool operator<(const ItemSet& rhs) const {
			return this->m_Set.size() < rhs.m_Set.size();
		}
		operator std::string() const {
			return this->toString();
		}
		operator std::vector<ItemT>() const {
			return this->getItemVector();
		}

		// NON-OPERATOR FUNCTIONS
		std::string toString() const {

			std::string str;

			for (const auto& item : this->m_Set)
				str += (std::string)item + "\n";

			if (str.empty())
				return "{ }";

			return (std::string)"{\n" + str + "}";
		};

		std::vector<ItemT> getItemVector() const { return this->m_Set; }
		
		const ItemT& at(size_t index) const { return this->m_Set.at(index); }
		auto begin() { return this->m_Set.begin(); }
		auto end() { return this->m_Set.end(); }
		auto begin() const { return this->m_Set.begin(); }
		auto end() const { return this->m_Set.end(); }
		size_t size() const { return this->m_Set.size(); }
		bool empty() const { return this->m_Set.empty(); }

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
		* @input the grammar to be used to get all the production for a non-terminal when the dot is before that non-terminal.
		* @return the CLOSURE set of this item set.
		*/
		ItemSet CLOSURE(ProdVecType*const);
		/**
		* @return the GOTO set of this item set on a particular symbol.
		*/
		ItemSet GOTO(const SymbolType&, ProdVecType*const);

	};
}

// IMPLEMENTATION
namespace m0st4fa {

	template<typename ItemT>
	bool ItemSet<ItemT>::_add_to_closure_no_lookaheads(const ProdVecType& symbolProductions)
	{
		bool inserted = false;
		this->p_Logger.logDebug(std::format("(Possibly) adding new items to the closure of an item set"));
		this->p_Logger.logDebug(std::format("Closure before:\n {}", stringfy(this->m_Closure, false)));

		// go through every production and insert the non-kernel items you create into the closure.
		for (const ProductionType& production : symbolProductions) {
			const ItemT item = ItemT{ production, 0 };

			// find an item with the same first component
			const auto it = this->get_item_it(item.production, item.dotPos, false);
			const auto end = this->m_Closure.end();
			bool found = it != end;

			// if no entry with the same first component is found
			if (not found) {
				this->m_Closure.push_back(item);
				inserted = true;
			} // if one was found, we cannot insert it again

		}
		this->p_Logger.logDebug(std::format("Closure after:\n {}", stringfy(this->m_Closure, false)));

		return inserted;
	}

	template<typename ItemT>
	bool ItemSet<ItemT>::_add_to_closure_lookaheads(const ProdVecType& symbolProductions, const LookAheadSet& lookaheads)
	{
		bool inserted = false;
		this->p_Logger.logDebug(std::format("Closure before:\n {}", stringfy(this->m_Closure, false)));

		// go through every production and insert the non-kernel items you create into the closure.
		for (const ProductionType& production : symbolProductions) {
			const ItemT item = ItemT{ production, 0 };

			// go through every production and insert the non-kernel items you create into the closure
			for (const ProductionType& production : symbolProductions) {

				// find an item with the same first component
				const auto it = this->get_item_it(production, 0, false);
				const auto end = this->m_Closure.end();
				bool found = it != end;

				// if an entry with the same first component is found
				if (found) {
					if (insertAndAssert(lookaheads, it->lookaheads))
						inserted = true;
				}
				else {
					// if no entry with the same first component is found
					const auto item = ItemT{ production, 0, lookaheads };
					this->m_Closure.push_back(item);
					inserted = true;
					this->p_Logger.logDebug(std::format("Adding new item {} to the closure of the item set", item.toString()));
				}

			}

		}		
		
		this->p_Logger.logDebug(std::format("Closure after:\n {}", stringfy(this->m_Closure, false)));

		return inserted;
	}

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

	template<typename ItemT>
	ItemSet<ItemT> ItemSet<ItemT>::CLOSURE(ProdVecType*const grammarPtr)
	{
		// check if the closure is already calculated
		if (this->m_Closure.size() > 0) {
			this->p_Logger.logDebug("CLOSURE set for this item set has already been calculated!");
			return this->m_Closure;
		}

		ProdVecType& grammar = *grammarPtr;
		// check if the set is not empty (its closure would therefore be empty)
		if (this->m_Set.size() == 0) {
			this->p_Logger.log(LoggerInfo::WARNING, "Item set is emtpy. Returning empty CLOSURE!");
			return *this;
		}

		// keep track of alternative productions of symbols
		std::vector<size_t> alternativeProduction[(size_t)VariableType::NT_COUNT]{};
		for (size_t prodIndex = 0; const ProductionType & prod : grammar.getVector()) {
			size_t varIndex = (size_t)prod.prodHead.as.nonTerminal;
			alternativeProduction[varIndex].push_back(prodIndex);
			prodIndex++;
		}

		this->p_Logger.logDebug("\nCALCULATING CLOSURE SET:\n");
		this->p_Logger.logDebug(std::format("Items set:\n {}", (std::string)*this));

		// initialize the closure to the item set
		this->m_Closure = ItemSet{this->m_Set};

		// check whether the items are LR(0) or they have lookaheads
		bool isLR0 = this->m_Closure.at(0).lookaheads.size() == 0;
		std::vector<std::set<SymbolType>> first{};
		if (not isLR0)
			grammar.calculateFIRST(), first = grammar.getFIRST();

		this->p_Logger.logDebug(std::format("The items in this set are LR({})", isLR0 ? "0" : "1"));

		// for every item of the closure, calculate the closure of that item
		for (size_t i = 0; i < this->m_Closure.size(); i++) {
			const ItemT item = this->m_Closure.at(i);
			this->p_Logger.logDebug(std::format("Current item being scanned for CLOSURE:{}",item.toString()));
			const ProductionType& itemProd = item.production;

			// if the dot is at the end of the production
			if (itemProd.size() == item.dotPos)
				continue; // there is nothing more to add

			// get the symbol before the dot; note that it is guaranteed to be a symbol
			// BUG: item.getActualDotPosition()
			const SymbolType& symbolAfterDot = itemProd.at(item.getActualDotPosition()).as.gramSymbol;
			this->p_Logger.logDebug(std::format("Symbol after dot is {}", symbolAfterDot.toString()));
			
			// check if this symbol is a non-terminal
			if (symbolAfterDot.isTerminal) {
				// if it is a terminal, the closure of the item is itself and the itemisalrea there in the closure (since we are checking it now)
				this->p_Logger.logDebug("No other item has been added to CLOSURE");
				continue;
			}

			// if we are here, the symbol is a non-terminal
			// get all the productions for that non-terminal (all of its alternatives)
			ProdVecType symbolProductions {};
			const std::vector<size_t>& prods = alternativeProduction[(size_t)symbolAfterDot.as.nonTerminal];
			for (size_t prodIndex : prods)
				symbolProductions.pushProduction(grammar.at(prodIndex));

			this->p_Logger.logDebug(std::format("The alternatives of non-terminal {} are: {}",symbolAfterDot.toString(), symbolProductions.toString()));

			// add the new items to the closure
			if (isLR0)
				_add_to_closure_no_lookaheads(symbolProductions);
			else // it is LR(1)
			{
				auto predIsGramSymbol = [](const auto& stackElement) {
					return stackElement.type == ProdElementType::PET_GRAM_SYMBOL;
				};

				// calculate the lookaheads
				size_t prodSize = itemProd.prodBody.size();
				size_t startIndex = item.getActualDotPosition() + 1;
				SymbolStringType symString{};
				
				// the following loop corresponds to: std::copy_if(begin + startIndex, end, symString.symbols, predIsGramSymbol);
				const auto& prodBody = itemProd.prodBody;
				for (size_t i = startIndex; i < prodBody.size(); i++) {
					const auto& se = prodBody.at(i);

					// if `se` is a grammar symbol, conncatenate its corresponding grammar symbol with `symString`
					if (predIsGramSymbol(se))
						symString.push_back(se.as.gramSymbol);

				}

				this->p_Logger.logDebug(std::format("For item {} => Initial string after the grammar symbol: {}", item.toString(), symString.toString()));

				// go through each lookahead; each lookahead corresponds to a distinct item
				for (const SymbolType& lookahead : item.lookaheads) {
					// the string for the current item
					SymbolStringType tempSymString = symString;
					tempSymString.push_back(lookahead);

					// the lookaheads of the current item

					tempSymString.calculateFIRST(first);
					LookAheadSet lookaheads{tempSymString.getFIRST()};

					this->p_Logger.logDebug(std::format("The lookaheads for the current item is: {}", stringfy(lookaheads)));

					// add the item to the CLOSURE set
					_add_to_closure_lookaheads(symbolProductions, lookaheads);
				}

			}
		}

		ItemSet res{ this->m_Closure, true };
		return res;
	}

	template<typename ItemT>
	ItemSet<ItemT> ItemSet<ItemT>::GOTO(const SymbolType& symbol, ProdVecType*const grammarPtr)
	{
		// check whether the closure is already calculated
		if (this->m_Closure.size() == 0) {
			this->p_Logger.log(LoggerInfo::INFO, "CLOSURE set is not already calculated for this set! Calculating it now.");

			this->CLOSURE(grammarPtr);
		}

		ProdVecType& grammar = *grammarPtr;
		this->p_Logger.logDebug("\nCALCULATING GOTO SET:\n");
		this->p_Logger.logDebug(std::format("Item set:\n {}", (std::string)*this));
		this->p_Logger.logDebug(std::format("GOTO symbol is {}", (std::string)symbol));

		ItemSet<ItemT> result = ItemSet{};

		/* assuming symbol is `B`
			figure out whether there exists an item of the form:
			[A -> alpha .B beta, a]
		*/
		ItemSet<ItemT> foundItems{};
		for (size_t i = 0; i < this->m_Closure.size(); i++) {
			auto predicate = [&symbol](const ItemT& item) {
				size_t dotPos = item.getActualDotPosition();

				// skip this item if the dot is at the end
				if (item.production.size() == dotPos)
					return false;

				const SymbolType& sym = item.production.at(dotPos).as.gramSymbol;
				return sym == symbol;
			};

			const ItemT currItem = this->m_Closure.at(i);

			// if symbolAfterDot == symbol
			if (predicate(currItem))
				foundItems.insert(currItem);
		}

		// figure out whether such an item exists
		bool found = foundItems.size() > 0;

		// if such an item is found
		if (found) {
			const auto& itemVec = foundItems.getItemVector();
			for (const ItemT& item : itemVec) {
				// create new item
				size_t newDotPos = item.dotPos + 1;

				// ERR: THE EXCEPTION IS HERE!!!
				ItemT kernelItem = ItemT{ item.production, newDotPos, item.lookaheads };

				// insert the item in `result`
				result.insert(kernelItem);
				this->p_Logger.log(LoggerInfo::DEBUG, std::format("Inserting kernel item {} to the GOTO set.", kernelItem.toString()));
			}

		}

		const ItemSet<ItemT> resultClosure = result.CLOSURE(grammarPtr);
		this->p_Logger.logDebug(std::format("GOTO Set for the items: \n{}", (std::string)resultClosure));
		return resultClosure;
	}

}
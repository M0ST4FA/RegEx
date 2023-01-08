#pragma once

#include "LRParser.hpp"

namespace m0st4fa {
	enum class ConflictPolicy {
		CP_PREFER_SHIFT, // prefer shift over reduce, but stop if there are two shifts
		CP_NONE, // stop always
		CP_COUNT
	};

	template <typename GrammarT>
	concept LRPGenConcept = requires (GrammarT grammar) {
		true;
		/*grammar.calculateFIRT();
		grammar.getFIRST();
		grammar.calculateFOLLOW();
		grammar.getFOLLOW();*/
	};

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	class LRParserGenerator {
		using ProdVecType = decltype(GrammarT{}.getProdVectorValue());
		using ProductionType = decltype(ItemT{}.production);
		using ProductionElemenType = decltype(ProductionType{}.prodBody.at(0));

		using SymbolType = decltype(ProductionType{}.prodHead);
		using VariableType = decltype(SymbolType{}.as.nonTerminal);
		using TerminalType = decltype(SymbolType{}.as.terminal);

		using SetType = decltype(GrammarT{}.getFOLLOW(SymbolType{}.as.nonTerminal));
		using LookAheadSetType = decltype(ItemT{}.lookaheads);

		using ItemSetType = ItemSet<ItemT>;
		using CollectionType = std::vector<ItemSetType>;

		// Assume FIRST set is stored within the grammar class
		SymbolType m_StartSymbol{};
		mutable LRParsingTableT m_ParsingTable{};
		GrammarT* m_Grammar{ nullptr };
		mutable bool m_ParsingTableGenerated = false;

		CollectionType _create_LR_collection(size_t = 0);
		void _create_parsing_table_lookahead(CollectionType&, ConflictPolicy cp);
		CollectionType _merge_sets_with_identical_cores(const CollectionType&);

		inline bool _check_entry_already_exists(const LRTableEntry& currEntry, const LRTableEntry& newEntry, size_t itemSetIndex, const ItemSetType& itemSet, const SymbolType& lookahead, ConflictPolicy cp, std::string_view grammarType = "SLR") const {
			/*
			* detect if there is already an entry at that location.
			* if that entry == the current entry, continue.
			* if that entry != the current entry, the location is being reset and that is an error.
			*/
			if (!(currEntry.type == LRTableEntryType::TET_ERROR)) {
				if (!(currEntry == newEntry))
					return _resolve_conflict(currEntry, newEntry, itemSetIndex, itemSet, lookahead, cp);
				//throw std::logic_error(grammarType + " table entry being redefined!");
				else
					return true;
			}

			return false;
		}

		inline bool _prompt_conflict_resolution(const LRTableEntry& currEntry, const LRTableEntry& newEntry, size_t itemSetIndex, const ItemSetType& itemSet, const SymbolType& lookahead) const {
			std::string msg1 = "PLease resolve the conflict in order to continue.";
			std::string iSet = itemSet.toString();
			std::string msg2 = "Would you like to replace the already existing entry with the new one? [type `yes` or `no`] ";
			this->p_Logger.log(LoggerInfo::FATAL_ERROR, std::format("Conflict: ACTION[{}][{}] = [{}], attempting to change it to: [{}]\n{}\nItem set: {}\n{}", itemSetIndex, lookahead.toString(), currEntry.toString(), newEntry.toString(), msg1, iSet, msg2));

			std::string response;
			std::cin >> response;

			if (response == "yes")
				return false;
			else if (response == "no")
				return true;
			else // TODO: SET THIS UP
				throw std::exception("Wrong answer");
		}

		inline bool _resolve_conflict(const LRTableEntry& currEntry, const LRTableEntry& newEntry, size_t itemSetIndex, const ItemSetType& itemSet, const SymbolType& lookahead, ConflictPolicy cp = ConflictPolicy::CP_NONE) const {
			switch (cp) {
			case ConflictPolicy::CP_NONE:
				return _prompt_conflict_resolution(currEntry, newEntry, itemSetIndex, itemSet, lookahead);
			case ConflictPolicy::CP_PREFER_SHIFT: {
				if (
					currEntry.type != LRTableEntryType::TET_ACTION_REDUCE && currEntry.type != LRTableEntryType::TET_ACTION_SHIFT
					) // prompt the user to resolve the conflict
					return _prompt_conflict_resolution(currEntry, newEntry, itemSetIndex, itemSet, lookahead);

				// the current entry is reduce and the new one is shift
				return false; // don't keep the current entry and replace it
			}
			default:
				throw std::exception("TODO: PUT A BETTER EXCEPTION HERE IN _resolve_conflict() when the conflict policy is unknown");
			}
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		inline bool _action_reduce_SLR(size_t i, const ItemSetType& currItemSet, const ItemT& currItem, ConflictPolicy cp) {
			const SymbolType& head = currItem.production.prodHead;
			[[maybe_unused]] const auto headNum = (size_t)head.as.nonTerminal;
			const std::set<SymbolType>& followHead = this->m_Grammar->getFOLLOW(head.as.nonTerminal);
			size_t prodNum = currItem.production.prodNumber;
			bool newEntryAdded = false;

			// set ACTION[i, a] to "reduce A --> a" for all a in FOLLOW(A)
			std::ranges::for_each(followHead.begin(), followHead.end(), [this, i, prodNum, &newEntryAdded, &currItemSet, cp](const SymbolType& sym) {
				LRTableEntry& currEntry = this->m_ParsingTable.atAction(i, sym.as.terminal);
			auto newEntry = LRTableEntry{ false, LRTableEntryType::TET_ACTION_REDUCE, prodNum };

			if (_check_entry_already_exists(currEntry, newEntry, i, currItemSet, sym, cp))
				return;

			this->p_Logger.log(LoggerInfo::INFO, std::format("ACTION[{}][{}] = R{}", i, (std::string)sym, prodNum));
			currEntry = newEntry;
			newEntryAdded = true;
				});

			return newEntryAdded;
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		inline bool _action_reduce_CLR(size_t i, const ItemSetType& currItemSet, const ItemT& currItem, ConflictPolicy cp) {
			const SymbolType& head = currItem.production.prodHead;
			[[maybe_unused]] const auto headNum = (size_t)head.as.nonTerminal;
			const LookAheadSetType& lookaheads = currItem.lookaheads;
			size_t prodNum = currItem.production.prodNumber;
			bool newEntryAdded = false;

			// set ACTION[i, a] to "reduce A --> a" for all a in lookahead set of the item.
			std::ranges::for_each(lookaheads.begin(), lookaheads.end(), [this, i, prodNum, &newEntryAdded, &currItemSet, cp](const SymbolType& sym) {
				LRTableEntry& currEntry = this->m_ParsingTable.atAction(i, sym.as.terminal);
			auto newEntry = LRTableEntry{ false, LRTableEntryType::TET_ACTION_REDUCE, prodNum };

			if (_check_entry_already_exists(currEntry, newEntry, i, currItemSet, sym, cp, "CLR/LALR"))
				return;

			this->p_Logger.log(LoggerInfo::INFO, std::format("ACTION[{}][{}] = R{}", i, (std::string)sym, prodNum));
			currEntry = newEntry;
			newEntryAdded = true;
				});

			return newEntryAdded;
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		inline bool _action_accept(size_t i, const ItemSetType& currItemSet, [[maybe_unused]] const ItemT& currItem) {
			LRTableEntry& currEntry = this->m_ParsingTable.atAction(i, TerminalType::T_EOF);
			auto newEntry = LRTableEntry{ false, LRTableEntryType::TET_ACCEPT };
			SymbolType symbol = to_symbol(TerminalType::T_EOF);

			if (_check_entry_already_exists(currEntry, newEntry, i, currItemSet, symbol, ConflictPolicy::CP_NONE, "SLR/CLR/LALR"))
				return false;

			currEntry = newEntry;
			this->p_Logger.log(LoggerInfo::INFO, std::format("ACTION[{}][{}] = ACCEPT", i, (std::string)symbol));
			return true;
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		inline bool _action_shift(size_t i, const ItemSetType& currItemSet, size_t j, const SymbolType& symAtDot, ConflictPolicy cp) {
			LRTableEntry& currEntry = this->m_ParsingTable.atAction(i, symAtDot.as.terminal);
			auto newEntry = LRTableEntry{ false, LRTableEntryType::TET_ACTION_SHIFT, j };

			if (_check_entry_already_exists(currEntry, newEntry, i, currItemSet, symAtDot, cp, "SLR/CLR/LALR"))
				return false;

			currEntry = newEntry;
			this->p_Logger.log(LoggerInfo::INFO, std::format("ACTION[{}][{}] = S{}", i, (std::string)symAtDot, j));
			return true;
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		inline bool _goto(size_t i, size_t j, const SymbolType& symAtDot) {
			LRTableEntry currEntry = this->m_ParsingTable.atGoto(i, symAtDot.as.nonTerminal);
			auto newEntry = LRTableEntry{ false, LRTableEntryType::TET_GOTO, j };
			if (_check_entry_already_exists(currEntry, newEntry, i, ItemSetType{}, SymbolType{}, ConflictPolicy::CP_NONE, "SLR/CLR/LALR"))
				return false;

			// if the symbol at the dot is a non-terminal
			this->m_ParsingTable.atGoto(i, symAtDot.as.nonTerminal) = newEntry;
			this->p_Logger.log(LoggerInfo::INFO, std::format("GOTO[{}][{}] = {}", i, (std::string)symAtDot, j));
			return true;
		};
		inline size_t _goto_set_number(ItemSetType& currItemSet, const SymbolType& symAtDot, const CollectionType& LRCollection) const {
			ItemSetType gotoSet = currItemSet.GOTO(symAtDot, this->m_Grammar);
			size_t j = LRCollection.size() - 1;

			for (; j > 0; j--)
				if (LRCollection.at(j).hasIdenticalCore(gotoSet))
					break;

			return j;
		}

	protected:
		Logger p_Logger;

		SymbolType to_symbol(const TerminalType terminal)
		{
			return SymbolType{ .isTerminal = true, .as {.terminal = terminal} };
		}
		SymbolType to_symbol(const VariableType variable)
		{
			return SymbolType{ .isTerminal = false, .as {.nonTerminal = variable} };
		}

		// TODO: CREATE DEFINITIONS FOR THESE
		static const SymbolType START_SYMBOL_PRIME;

	public:
		LRParserGenerator() = default;
		LRParserGenerator(const LRParserGenerator&) = default;
		LRParserGenerator(LRParserGenerator&&) noexcept = default;
		LRParserGenerator(const GrammarT& grammar, const SymbolType& startSymbol) : m_ParsingTable{ grammar }, m_Grammar{ &this->m_ParsingTable.grammar }, m_StartSymbol{ startSymbol } {
			this->m_ParsingTable.grammar.calculateFIRST();
			this->m_ParsingTable.grammar.calculateFOLLOW();
		};

		const GrammarT& getGrammar() {
			return this->m_Grammar;
		}
		SymbolType get_start_symbol() { return this->m_StartSymbol; }

		const LRParsingTableT& generateSLRParser(ConflictPolicy = ConflictPolicy::CP_NONE);
		const LRParsingTableT& generateCLRParser(ConflictPolicy = ConflictPolicy::CP_NONE);
		const LRParsingTableT& generateLALRParser(ConflictPolicy = ConflictPolicy::CP_NONE);
	};

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const typename LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::SymbolType
		LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::START_SYMBOL_PRIME{ .isTerminal = false, .as = {.nonTerminal = SIZE_MAX - 1} };
}

namespace m0st4fa {
	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::CollectionType LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::_create_LR_collection(size_t lookaheadNum)
	{
		// check for whether the number of lookaheads is correct or no
		if (lookaheadNum > 1) {
			const std::string msg = std::format("Incorrect number of lookaheads `{}`! Supported lookaheads are `0` and `1`.", lookaheadNum);
			this->p_Logger.log(LoggerInfo::ERR_INVALID_VAL, msg);
			throw std::logic_error("Incorrect number of lookaheads!");
		}

		CollectionType resCollection;

		// Initialize the collection of LR(0) item sets
		const ProductionType startProd = this->m_ParsingTable.grammar.at(0);
		ItemT startItem{ startProd, 0 };
		if (lookaheadNum == 1)
			startItem.lookaheads = { SymbolType::END_MARKER };

		ItemSetType startItemSet{ startItem };
		resCollection.push_back(startItemSet.CLOSURE(this->m_Grammar));

		// Populate the collection of LR(0) item sets
		for (size_t itemSetIndex = 0; itemSetIndex < resCollection.size(); itemSetIndex++) {
			ItemSetType itemSet = resCollection.at(itemSetIndex);

			for (const ItemT item : itemSet) {
				const SymbolType& sym = item.symbolAtDotPosition();

				// if the dot is at the end of the item
				if (item.isDotPositionAtEnd())
					continue; // move on to the next item

				ItemSetType gotoSet = itemSet.GOTO(sym, this->m_Grammar);

				if (gotoSet.empty())
					continue;

				// check whether this item set already exists
				if (bool alreadyExists = std::any_of(
					resCollection.rbegin(),
					resCollection.rend(),
					[&gotoSet](const ItemSetType& is) {
						return gotoSet == is;
					}
				); alreadyExists)
					continue;

				this->p_Logger.logDebug(std::format("Inserting item set to the LR(0) collection: {}", (std::string)gotoSet));
				resCollection.push_back(gotoSet);
			}
		}

		return resCollection;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	void LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::_create_parsing_table_lookahead(CollectionType& LRCollection, ConflictPolicy cp)
	{
		const ProductionType startProd = this->m_ParsingTable.grammar.at(0);
		const ItemT endItem{ startProd, 1, {to_symbol(TerminalType::T_EOF)} };

		for (size_t i = 0; i < LRCollection.size(); i++) {
			ItemSetType& currItemSet{ LRCollection.at(i) };

			for (const ItemT& currItem : currItemSet) {
				// if the dot is at the end of the production
				if (currItem.isDotPositionAtEnd()) {
					// check whether this item is the end item
					if (currItem == endItem)
						_action_accept(i, currItemSet, currItem);
					else // if this item is not the end item
						_action_reduce_CLR(i, currItemSet, currItem, cp);

					continue; // we're finished with this item; move on to the next one
				}

				// if the dot is not at the end of the production
				const SymbolType& symAtDot = currItem.symbolAtDotPosition();
				size_t j = _goto_set_number(currItemSet, symAtDot, LRCollection);

				// if the symbol at the dot is a terminal
				if (symAtDot.isTerminal)
					_action_shift(i, currItemSet, j, symAtDot, cp);
				else // if the symbol at the dot is a non-terminal
					_goto(i, j, symAtDot);
			}
		}
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::CollectionType
		LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::_merge_sets_with_identical_cores(const CollectionType& LR1Collection)
	{
		CollectionType res;
		size_t LR1CollSize = LR1Collection.size();
		std::vector<bool> alreadyMerged(LR1CollSize, false);

		for (size_t setIndex = 0; setIndex < LR1CollSize; setIndex++) {
			ItemSetType currSet = LR1Collection.at(setIndex);

			// check whether the set has already been merged
			if (alreadyMerged.at(setIndex))
				continue;

			// if the set is not already merged, find all sets with identical cores and merge them with it
			// note that if the set is not already merged, this means that all sets with identical cores are also not already merged
			for (size_t j = setIndex + 1; j < LR1CollSize; j++) {
				const ItemSetType& s = LR1Collection.at(j);

				if (not currSet.hasIdenticalCore(s))
					continue;

				alreadyMerged.at(j) = true;
				currSet.merge(s);
			}

			res.push_back(currSet);
		}

		return res;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const LRParsingTableT& LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::generateSLRParser(ConflictPolicy conflictPolicy)
	{
		/** Algorithm
		* Create the set of LR(0) items.
			* Each item set of the collection represents state i, i is the index of the item set.
		* For every item set i (representing state i):
			* If i contains an item of the form [S -> S'., $]:
				* Set ACTION[i][$] = ACCEPT.
			* If i contains an item of the form [A -> XYZ.b, a]:
				* Set ACTION[i][b] = j, j is the item set CLOSURE([A -> XYZb., a]).
			* If i contains an item of the form [A -> XYZ.B, a]:
				* Set GOTO[i][B] = j, j is the item set CLOSURE([A -> XYZB., a]).
		*/

		const ProductionType startProd = this->m_ParsingTable.grammar.at(0);
		const ItemT endItem{ startProd, 1 };
		CollectionType LR0Collection = _create_LR_collection();
		this->p_Logger.log(LoggerInfo::INFO, std::format("LR(0) COLLECTION:\n{}", toString(LR0Collection)));

		for (size_t i = 0; i < LR0Collection.size(); i++) {
			ItemSetType& currItemSet{ LR0Collection.at(i) };

			for (const ItemT& currItem : currItemSet) {
				// if the dot is at the end of the production
				if (currItem.isDotPositionAtEnd()) {
					// check whether this item is the end item
					if (currItem == endItem)
						_action_accept(i, currItemSet, currItem);
					else // if this item is not the end item
						_action_reduce_SLR(i, currItemSet, currItem, conflictPolicy);

					continue; // we're finished with this item; move on to the next one
				}

				// if the dot is not at the end of the production
				const SymbolType& symAtDot = currItem.symbolAtDotPosition();
				size_t j = _goto_set_number(currItemSet, symAtDot, LR0Collection);

				// if the symbol at the dot is a terminal
				if (symAtDot.isTerminal)
					_action_shift(i, currItemSet, j, symAtDot, conflictPolicy);
				else // if the symbol at the dot is a non-terminal
					_goto(i, j, symAtDot);
			}
		}

		return this->m_ParsingTable;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const LRParsingTableT& LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::generateCLRParser(ConflictPolicy conflictPolicy)
	{
		CollectionType LR1Collection = _create_LR_collection(1);
		this->p_Logger.log(LoggerInfo::INFO, std::format("LR(1) COLLECTION:\n{}\nSize of collection: {}", toString(LR1Collection), LR1Collection.size()));

		_create_parsing_table_lookahead(LR1Collection, conflictPolicy);

		return this->m_ParsingTable;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const LRParsingTableT& LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::generateLALRParser(ConflictPolicy conflictPolicy)
	{
		// generate LR(1) collection
		const ProductionType startProd = this->m_ParsingTable.grammar.at(0);
		const ItemT endItem{ startProd, 1, {to_symbol(TerminalType::T_EOF)} };
		CollectionType LR1Collection = _create_LR_collection(1);
		this->p_Logger.log(LoggerInfo::INFO, std::format("LR(1) COLLECTION:\n{}\nSize of collection: {}", toString(LR1Collection), LR1Collection.size()));

		/**
		* obtain LALR(1) collection by merging sets with identical cores
		*/
		CollectionType LALR1Collection = _merge_sets_with_identical_cores(LR1Collection);
		this->p_Logger.log(LoggerInfo::INFO, std::format("LALR(1) COLLECTION:\n{}\nSize of collection: {}", toString(LALR1Collection), LALR1Collection.size()));

		// generate the parsing table (in the same way as a CLR(1) parser)
		_create_parsing_table_lookahead(LALR1Collection, conflictPolicy);

		return this->m_ParsingTable;
	}
}

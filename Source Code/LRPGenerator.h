#pragma once

#include "LRParser.hpp"

namespace m0st4fa {

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

		using ItemSetType = ItemSet<ItemT>;
		using CollectionType = std::vector<ItemSetType>;

		// Assume FIRST set is stored within the grammar class
		SymbolType m_StartSymbol{};
		mutable LRParsingTableT m_ParsingTable{};
		GrammarT* m_Grammar{nullptr};
		mutable bool m_ParsingTableGenerated = false;

		CollectionType _create_LR0_collection();
		bool _check_entry_already_exists(const LRTableEntry& currEntry, const LRTableEntry& newEntry) const {
			/*
			* detect if there is already an entry at that location.
			* if that entry == the current entry, continue.
			* if that entry != the current entry, the location is being reset and that is an error.
			*/
			if (not (currEntry.type == LRTableEntryType::TET_ERROR))
				if (not (currEntry == newEntry)) {
					this->p_Logger.log(LoggerInfo::ERR_UNACCEPTED_GRAMMAR, std::format("This grammar is not SLR(1); SLR table entry being redefined!"));
					throw std::logic_error("SLR table entry being redefined!");
				}
				else
					return true;

			return false;
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		bool _action_reduce_SLR(size_t i, const ItemT& currItem) {
			const SymbolType& head = currItem.production.prodHead;
			const size_t headNum = (size_t)head.as.nonTerminal;
			const std::set<SymbolType>& followHead = this->m_Grammar->getFOLLOW(head.as.nonTerminal);
			size_t prodNum = currItem.production.prodNumber;
			bool newEntryAdded = false;

			// set ACTION[i, a] to "reduce A --> a" for all a in FOLLOW(A)
			std::for_each(followHead.begin(), followHead.end(), [this, i, prodNum, &newEntryAdded](const SymbolType& sym) {
				LRTableEntry& currEntry = this->m_ParsingTable.atAction(i, sym.as.terminal);
				LRTableEntry newEntry = LRTableEntry{ false, LRTableEntryType::TET_ACTION_REDUCE, prodNum };

				if (_check_entry_already_exists(currEntry, newEntry))
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
		bool _action_shift(size_t i, size_t j, const SymbolType& symAtDot) {
			LRTableEntry& currEntry = this->m_ParsingTable.atAction(i, symAtDot.as.terminal);
			LRTableEntry newEntry = LRTableEntry{ false, LRTableEntryType::TET_ACTION_SHIFT, j };

			if (_check_entry_already_exists(currEntry, newEntry))
				return false;

			currEntry = newEntry;
			this->p_Logger.log(LoggerInfo::INFO, std::format("ACTION[{}][{}] = S{}", i, (std::string)symAtDot, j));
			return true;
		}
		/**
		* @return bool representing whether a new entry has been added
		*/
		bool _goto(size_t i, size_t j, const SymbolType& symAtDot) {
			LRTableEntry currEntry = this->m_ParsingTable.atGoto(i, symAtDot.as.nonTerminal);
			LRTableEntry newEntry = LRTableEntry{ false, LRTableEntryType::TET_GOTO, j };
			if (_check_entry_already_exists(currEntry, newEntry))
				return false;

			// if the symbol at the dot is a non-terminal
			this->m_ParsingTable.atGoto(i, symAtDot.as.nonTerminal) = newEntry;
			this->p_Logger.log(LoggerInfo::INFO, std::format("GOTO[{}][{}] = {}", i, (std::string)symAtDot, j));
			return true;
		};
		size_t _goto_set_number(ItemSetType& currItemSet, const SymbolType& symAtDot, const CollectionType& LR0Collection) const {
			ItemSetType gotoSet = currItemSet.GOTO(symAtDot, this->m_Grammar);
			size_t j = LR0Collection.size() - 1;

			for (; j >= 0; j--)
				if (LR0Collection.at(j) == gotoSet)
					break;

			return j;
		}

		void _fill_table(const ProductionType&, size_t) const;

	protected:
		Logger p_Logger{};

		// TODO: CREATE DEFINITIONS FOR THESE
		static const SymbolType START_SYMBOL_PRIME;

	public:
		LRParserGenerator() = default;
		LRParserGenerator(const LRParserGenerator&) = default;
		LRParserGenerator(LRParserGenerator&&) = default;
		LRParserGenerator(const GrammarT& grammar, const SymbolType& startSymbol) : m_ParsingTable{ grammar }, m_Grammar{ &this->m_ParsingTable.grammar }, m_StartSymbol{ startSymbol } {
			this->m_ParsingTable.grammar.calculateFIRST();
			this->m_ParsingTable.grammar.calculateFOLLOW();
		};

		const GrammarT& getGrammar() {
			return this->m_Grammar;
		}
		SymbolType getStartSymbol() { return this->m_StartSymbol; }

		const LRParsingTableT& generateSLRParser();
		const LRParsingTableT& generateCLRParser() const;
		const LRParsingTableT& generateLALRParser() const;
	};


	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const typename LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::SymbolType
	LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::START_SYMBOL_PRIME{ .isTerminal = false, .as = {.nonTerminal = SIZE_MAX - 1} };
	
}


namespace m0st4fa {

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::CollectionType LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::_create_LR0_collection()
	{
		CollectionType resCollection;
		
		// Initialize the collection of LR(0) item sets
		const ProductionType startProd = this->m_ParsingTable.grammar.at(0);		
		const ItemT startItem{ startProd, 0 };
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
				bool alreadyExists = std::any_of(
					resCollection.rbegin(),
					resCollection.rend(),
					[&gotoSet](const ItemSetType& is) {
						return gotoSet == is;
					}
				);

				if (alreadyExists)
					continue;

				this->p_Logger.logDebug(std::format("Inserting item set to the LR(0) collection: {}", (std::string)gotoSet));
				resCollection.push_back(gotoSet);
			}

		}

		return resCollection;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const LRParsingTableT& LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::generateSLRParser()
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
		CollectionType LR0Collection = _create_LR0_collection();
		this->p_Logger.log(LoggerInfo::INFO, std::format("LR(0) COLLECTION:\n{}", stringfy(LR0Collection)));

		for (size_t i = 0; i < LR0Collection.size(); i++) {
			ItemSetType& currItemSet{ LR0Collection.at(i) };
			
			for (const ItemT& currItem : currItemSet) {

				// if the dot is at the end of the production
				if (currItem.isDotPositionAtEnd()) {
					// check whether this item is the end item
					if (currItem == endItem) {
						this->p_Logger.log(LoggerInfo::INFO, std::format("ACTION[{}][{}] = ACCEPT", i, (std::string)toSymbol(TerminalType::T_EOF)));
						this->m_ParsingTable.atAction(i, TerminalType::T_EOF) = LRTableEntry{ false, LRTableEntryType::TET_ACCEPT };
					}
					else // if this item is not the end item
						_action_reduce_SLR(i, currItem);
					
					continue; // we're finished with this item; move on to the next one
				}

				// if the dot is not at the end of the production
				const SymbolType& symAtDot = currItem.symbolAtDotPosition();
				size_t j = _goto_set_number(currItemSet, symAtDot, LR0Collection);
				
				// if the symbol at the dot is a terminal
				if (symAtDot.isTerminal)
					_action_shift(i, j, symAtDot);
				else // if the symbol at the dot is a non-terminal
					_goto(i, j, symAtDot);

			}

		}

		return this->m_ParsingTable;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const LRParsingTableT& LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::generateCLRParser() const
	{	
		this->p_Logger.log(LoggerInfo::FATAL_ERROR, "generateCLRParser is not yet finished");
		std::abort();
		return this->m_ParsingTable;
	}

	template<typename GrammarT, typename ItemT, typename LRParsingTableT>
		requires LRPGenConcept<GrammarT>
	const LRParsingTableT& LRParserGenerator<GrammarT, ItemT, LRParsingTableT>::generateLALRParser() const
	{
		this->p_Logger.log(LoggerInfo::FATAL_ERROR, "generateLALRParser is not yet finished");
		std::abort();
		return this->m_ParsingTable;
	}

}

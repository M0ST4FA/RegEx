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
		const ItemT startItem{ startProd, 0, LookAheadSet<SymbolType>{toSymbol(TerminalType::T_EOF)} };
		ItemSetType startItemSet{ startItem };
		resCollection.push_back(startItemSet.CLOSURE(this->m_Grammar));

		// Populate the collection of LR(0) item sets
		for (ItemSetType itemSet : resCollection) {

			this->p_Logger.logDebug(std::format("The current item set:\n{}", (std::string)itemSet));

			for (const ItemT item : itemSet) {

				const SymbolType& sym = item.symbolAtDotPosition();
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

		CollectionType LR0Collection = _create_LR0_collection();
		this->p_Logger.logDebug(std::format("LR(0) COLLECTION:\n{}", stringfy(LR0Collection)));

		this->p_Logger.log(LoggerInfo::FATAL_ERROR, "generateSLRParser is not yet finished");
		std::abort();
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

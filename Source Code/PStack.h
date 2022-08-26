#pragma once
#include <vector>

#include "PEnum.h"

// defined constants
#define EXTRACT_TERMINAL(stackElement) (size_t)stackElement.as.gramSymbol.as.terminal
#define EXTRACT_VARIABLE(stackElement) (size_t)stackElement.as.gramSymbol.as.nonTerminal

namespace m0st4fa {
	
	// FORWARD DECLARATIONS
	template <typename DataT>
	struct SynthesizedRecord;

	template <typename DataT>
	struct ActionRecord;

	template <
		typename SymbolT, 
		typename SynthesizedT, 
		typename ActionT
	>
		requires requires (SymbolT sym1, SymbolT sym2) { sym1 = sym2; }
	struct StackElement {

		StackElementType type;

		union {
			SymbolT gramSymbol;
			SynthesizedT synRecord;
			ActionT actRecord;
		} as;

		StackElement& operator=(const StackElement& other) {
			type = other.type;
			switch (type) {

			case SET_GRAM_SYMBOL:
				as.gramSymbol = other.as.gramSymbol;
				break;

			case SET_SYNTH_RECORD:
				as.synRecord = other.as.synRecord;
				break;

			case SET_ACTION_RECORD:
				as.actRecord = other.as.actRecord;
				break;
			}

			return *this;
		}
		bool operator==(const StackElement& other) {

			if (this->type != other.type)
				return false;

			switch (type) {

			case SET_GRAM_SYMBOL:
				return as.gramSymbol == other.as.gramSymbol;

			case SET_SYNTH_RECORD:
				return as.synRecord == other.as.synRecord;

			case SET_ACTION_RECORD:
				return as.actRecord == other.as.actRecord;

			}

			return false;
		}
		
	};

	template <
		typename SymbolT,
		typename SynthesizedT,
		typename ActionT
	>
	using Stack = std::vector<StackElement<SymbolT, SynthesizedT, ActionT>>;

	/**
	* @brief A simple base class for synthesized records from which you can derive more complex classes.
	*/
	template <typename DataT>
	struct SynthesizedRecord {
		DataT data;

		/**
		* This is expected to be a pointer to an action taking a Stack and Data as input
		*/
		void* action = nullptr;

		bool operator==(const SynthesizedRecord other) {
			return this->action == other.action;
		}

	};



	/**
	* @brief A simple base class for action records from which you can derive more complex classes.
	*/
	template <typename DataT>
	struct ActionRecord {
		DataT data;

		/**
		* This is expected to be a pointer to an action taking a Stack and Data as input
		*/
		void* action = nullptr;

		bool operator==(const ActionRecord other) {
			return this->action == other.action;
		}

	};

}
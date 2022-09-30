#pragma once
#include <vector>
#include <type_traits>
#include <sstream>

#include "PEnum.h"

// defined constants
#define EXTRACT_TERMINAL(stackElement) (size_t)stackElement.as.gramSymbol.as.terminal
#define EXTRACT_VARIABLE(stackElement) (size_t)stackElement.as.gramSymbol.as.nonTerminal

namespace m0st4fa {
	
	// FORWARD DECLARATIONS
	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct SynthesizedRecord;

	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct ActionRecord;

	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	concept StackElementConstraints = requires (SymbolT sym1, SymbolT sym2) { sym1 = sym2; } && std::is_trivially_destructible_v<SynthesizedT> && std::is_trivially_destructible_v<ActionT>;

	template <
		typename SymbolT,
		typename SynthesizedT,
		typename ActionT
	>
		requires StackElementConstraints<SymbolT, SynthesizedT, ActionT>
	struct StackElement {

		StackElementType type;

		// TODO: try enhance this
		struct {
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
		bool operator==(const StackElement& other) const {

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
		
		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {
			static_assert(StackElementType::SET_COUNT == 3);

			switch (this->type) {
			case SET_GRAM_SYMBOL:
				return this->as.gramSymbol.toString();

			case SET_SYNTH_RECORD:
				return this->as.synRecord.toString();
				

			case SET_ACTION_RECORD:
				return this->as.actRecord.toString();
				
			default:
				return "Unreachable: Stack element un recognized";
			}

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
		requires requires (DataT data) { std::string(data); data == data; }
	struct SynthesizedRecord {
		DataT data;

		/**
		* This is expected to be a pointer to an action taking a Stack and Data as input
		*/
		void* action = nullptr;

		operator std::string() const {
			std::stringstream ss;
			ss << action;

			return std::string("{ data: ") + std::string(data) + ", action: 0x" + ss.str() + " }";
		}

		std::string toString() const {
			return std::string(*this);
		}

		bool operator==(const SynthesizedRecord& other) const {
			return this->action == other.action && this->data == other.data;
		}

	};



	/**
	* @brief A simple base class for action records from which you can derive more complex classes.
	*/
	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct ActionRecord {
		DataT data;

		/**
		* This is expected to be a pointer to an action taking a Stack and Data as input
		*/
		void* action = nullptr;

		operator std::string() const {
			std::stringstream ss;
			ss << action;

			return std::string("{ data: ") + std::string(data) + ", action: 0x" + ss.str() + " }";
		}

		std::string toString() const {
			return std::string(*this);
		}

		bool operator==(const ActionRecord& other) const {
			return this->action == other.action && this->data == other.data;
		}

	};

}
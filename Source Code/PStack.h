#pragma once
#include <vector>
#include <type_traits>
#include <sstream>

#include "PEnum.h"

// defined constants
#define EXTRACT_TERMINAL(stackElement) (size_t)stackElement.as.gramSymbol.as.terminal
#define EXTRACT_VARIABLE(stackElement) (size_t)stackElement.as.gramSymbol.as.nonTerminal

// SHARED (LL AND LR)
namespace m0st4fa {

	template <typename StackElementT>
	using StackType = std::vector<StackElementT>;

}

// LL PARSING
namespace m0st4fa {

	// FORWARD DECLARATIONS
	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct LLSynthesizedRecord;

	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct LLActionRecord;

	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	concept StackElementConstraints = requires (SymbolT sym1, SymbolT sym2) { sym1 = sym2; } && std::is_trivially_destructible_v<SynthesizedT> && std::is_trivially_destructible_v<ActionT>;

	template <
		typename SymbolT,
		typename SynthesizedT,
		typename ActionT
	>
		requires StackElementConstraints<SymbolT, SynthesizedT, ActionT>
	struct LLStackElement {
		
		ProdElementType type;

		// TODO: try enhance this
		struct {
			SymbolT gramSymbol;
			SynthesizedT synRecord;
			ActionT actRecord;
		} as;

		LLStackElement& operator=(const LLStackElement& other) {
			type = other.type;
			switch (type) {

			case ProdElementType::PET_GRAM_SYMBOL:
				as.gramSymbol = other.as.gramSymbol;
				break;

			case ProdElementType::PET_SYNTH_RECORD:
				as.synRecord = other.as.synRecord;
				break;

			case ProdElementType::PET_ACTION_RECORD:
				as.actRecord = other.as.actRecord;
				break;
			}

			return *this;
		}
		bool operator==(const LLStackElement& other) const {

			if (this->type != other.type)
				return false;

			switch (type) {

			case ProdElementType::PET_GRAM_SYMBOL:
				return as.gramSymbol == other.as.gramSymbol;

			case ProdElementType::PET_SYNTH_RECORD:
				return as.synRecord == other.as.synRecord;

			case ProdElementType::PET_ACTION_RECORD:
				return as.actRecord == other.as.actRecord;

			}

			return false;
		}
		
		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {
			static_assert((size_t)ProdElementType::PET_COUNT == 3);

			switch (this->type) {
			case ProdElementType::PET_GRAM_SYMBOL:
				return this->as.gramSymbol.toString();

			case ProdElementType::PET_SYNTH_RECORD:
				return this->as.synRecord.toString();
				

			case ProdElementType::PET_ACTION_RECORD:
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
	std::string stringfy(
		typename LLStackElement<SymbolT, SynthesizedT, ActionT>::StackElementType type) 
	{
		using StackElementType = LLStackElement<SymbolT, SynthesizedT, ActionT>::StackElementType;

		static_assert(StackElementType::SET_COUNT == 3);
		static constexpr const char* const names[] = {
			"GRAM_SYMBOL",
			"SYNTH_RECORD",
			"ACTION_RECORD",
		};


		const char* name = type == StackElementType::SET_COUNT ?
			std::to_string((unsigned)StackElementType::SET_COUNT).data() : names[static_cast<int>(type)];

		return name;
	};

	template <
		typename SymbolT,
		typename SynthesizedT,
		typename ActionT
	>
	std::ostream& operator<<(std::ostream& os, 
		typename LLStackElement<SymbolT, SynthesizedT, ActionT>::StackElementType type) {
		return os << stringfy(type);
	};

	/**
	* @brief A simple base class for synthesized records from which you can derive more complex classes.
	*/
	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct LLSynthesizedRecord {
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

		bool operator==(const LLSynthesizedRecord& other) const {
			return this->action == other.action && this->data == other.data;
		}

	};

	/**
	* @brief A simple base class for action records from which you can derive more complex classes.
	*/
	template <typename DataT>
		requires requires (DataT data) { std::string(data); data == data; }
	struct LLActionRecord {
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

		bool operator==(const LLActionRecord& other) const {
			return this->action == other.action && this->data == other.data;
		}

	};

}

// LR PARSING 
namespace m0st4fa {

	template<typename DataT, typename TokenT>
	struct LRState;
	using lrstate_t = size_t;
	template<typename DataT, typename TokenT>
	using LRStackType = StackType<LRState<DataT, TokenT>>;

	template<typename SymbolT>
	struct LRProductionElement {
		union {
			SymbolT gramSymbol;
		} as;
		ProdElementType type = ProdElementType::PET_GRAM_SYMBOL;

		LRProductionElement() = default;
		LRProductionElement(const LRProductionElement&) = default;
		LRProductionElement(const SymbolT& symbol) : as{ symbol } {};

		operator std::string() const {
			return this->toString();
		};
		bool operator==(const LRProductionElement& rhs) const {
			return this->type == rhs.type && this->as.gramSymbol == rhs.as.gramSymbol;
		}

		std::string toString() const {
			return this->as.gramSymbol.toString();
		}

	};

	template<typename DataT, typename TokenT>
	struct LRState {
		lrstate_t state = SIZE_MAX;
		DataT data{};
		TokenT token = TokenT::EPSILON;

		// methods
		LRState() = default;
		LRState(lrstate_t state, const DataT& data) : state{ state }, data{ data } {};
		LRState(lrstate_t state) : state{ state } {};

		operator std::string() const {
			return this->toString();
		}
		bool operator==(const LRState& rhs) {
			return this->state == rhs.state && this->data == rhs.data && token == token;
		}
		bool hasData() const {
			return (bool)data;
		}


		std::string toString() const {
			std::string res = std::format("<{}", state);

			if (this->hasData())
				res += ", " + (std::string)this->data;

			res += ">";

			return res;
		}

	};
}
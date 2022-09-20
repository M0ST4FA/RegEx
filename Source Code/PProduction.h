#pragma once
#include <string>
#include <vector>
// TODO: change the implementation to use unordered_set since it has linear access time and is a lot faster.
#include <set>

#include "PStack.h"
#include "common.h"

namespace m0st4fa {

	// Production
	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	struct ProductionRecord {

		using StackElement = StackElement<SymbolT, SynthesizedT, ActionT>;
		using Stack = Stack<SymbolT, SynthesizedT, ActionT>;

		// TODO: demand that the prodHead be a non-terminal
		SymbolT prodHead = SymbolT{};
		std::vector<StackElement> prodBody;

		std::string toString() const {

			std::string str = this->prodHead.toString() + " -> ";

			// body
			for (const StackElement& symbol : this->prodBody) {

				switch (symbol.type) {
				case SET_GRAM_SYMBOL:
					str += symbol.as.gramSymbol.toString() + " ";
					break;

				case SET_SYNTH_RECORD:
					str += symbol.as.synRecord.toString() + " ";
					break;

				case SET_ACTION_RECORD:
					str += symbol.as.actRecord.toString() + " ";
					break;

				}
			}


			return str;
		}

		// operators
		ProductionRecord& operator=(const ProductionRecord& other) {
			prodHead = other.prodHead;
			prodBody = other.prodBody;
			return *this;
		}

		operator std::string() const {
			return this->toString();
		}
	};

	template <typename SymbolT, typename SynthesizedT, typename ActionT>
	std::ostream& operator<<(std::ostream& os, const ProductionRecord<SymbolT, SynthesizedT, ActionT>& prod) {

		return std::cout << prod.toString() << "\n";
	};

	// Symbol
	template <typename TerminalT, typename VariableT>
	struct GrammaticalSymbol {
		bool isTerminal = false;

		union {
			TerminalT terminal;
			VariableT nonTerminal;
		} as;
		
		static const GrammaticalSymbol EPSILON;
		static const GrammaticalSymbol END_MARKER;

		template <typename TokenT>
			requires requires (TokenT tok) { tok.name; }
		bool operator==(const TokenT& token) const {

			if (this->isTerminal)
				return this->as.terminal == token.name;

			return false;
		}

		bool operator==(const GrammaticalSymbol& other) const {

			if (isTerminal)
				return as.terminal == other.as.terminal;
			else
				return as.nonTerminal == other.as.nonTerminal;
			
		};
		
		operator std::string() const {
			return this->toString();
		}

		std::string toString() const {

			return this->isTerminal ? stringfy(this->as.terminal) : stringfy(this->as.nonTerminal);

		}

	};

	template <typename TerminalT, typename VariableT>
	const GrammaticalSymbol<TerminalT, VariableT>
	GrammaticalSymbol<TerminalT, VariableT>::EPSILON = { true, {.terminal = TerminalT::T_EPSILON} };

	template <typename TerminalT, typename VariableT>
	const GrammaticalSymbol<TerminalT, VariableT>
		GrammaticalSymbol<TerminalT, VariableT>::END_MARKER = { true, {.terminal = TerminalT::T_EOF} };

	template <typename TerminalT, typename VariableT>
	using Symbol = GrammaticalSymbol<TerminalT, VariableT>;

	template <typename TerminalT, typename VariableT>
	std::ostream& operator<<(std::ostream& os, const Symbol<TerminalT, VariableT>& symbol) {

		return os << symbol.toString();
		
	}

	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	class ProductionVector {
		using ProdRec = ProductionRecord<SymbolT, SynthesizedT, ActionT>;
		using VecType = std::vector<ProdRec>;
		// TODO: consider making this use terminals instead for storage efficiencey
		using SetType = std::vector<std::set<SymbolT>>;


		/**
		* The index of the non-terminal will hold its FIRST or FOLLOW set.
		*/
		SetType FIRST {0};
		SetType FOLLOW {0};
		
		bool m_CalculatedFIRST = false;
		bool m_CalculatedFOLLOW = false;

		/**
		* Calculates FIRST for the head of a production, using whatever information is currently available.
		* @return true = continue to the next grammar symbol, false = continue to the next production;
		*/
		bool _calc_FIRST_of_prod(const ProdRec&, const SymbolT&, bool&, size_t);
		/**
		* Augments the follow set of nonterminal N, if possible with currently available data.
		* @param nonTerminal: VariableT, prodIndex: size_t, nonTerminalIndex: size_t
		* @return void
		*/
		bool _calc_FOLLOW_of_nonTerminal(decltype(SymbolT().as.nonTerminal), size_t, size_t);

	protected:
		VecType m_Vector{};
		Logger m_Logger{};

	public:

		// constructors
		ProductionVector() = default;
		ProductionVector(const VecType& vec) : m_Vector { vec } {};

		// production vector access methods
		const VecType& getProdVector() { return this->m_Vector; }
		void pushProduction(const ProdRec& prod) { this->m_Vector.push_back(prod); }

		// element access methods
		const ProdRec& operator [] (size_t i) { return this->m_Vector.at(i); }
		const ProdRec& at(size_t i) { return *this[i]; };

		// conversions methods
		operator std::string() {
			return this->toString();
		}
		std::string toString() {

			std::string str;

			for (const auto& prod : this->getProdVector())
				str += (std::string)prod + "\n";

			return str;
		}

		// FIRST and FOLLOW calculation methods
		/**
		* Calculates FIRST for all non-terminals of this production vector.
		* @return true if FIRST is calculated, otherwise, calculates FIRST and then returns true.
		*/
		bool calculateFIRST();
		bool FIRSTCalculated() { return this->m_CalculatedFIRST; };
		std::set<SymbolT> getFIRST(decltype(SymbolT().as.nonTerminal) nonTerminal) { 
				
			// if FIRST is already calculated
			if (this->m_CalculatedFIRST)
				return this->FIRST[(size_t)nonTerminal];

			// handle the nonpresence of the FIRST set for this production vector
			this->m_Logger.log(LoggerInfo::ERR_MISSING_VAL, "The FIRST set of the non-terminals of this production vector is yet to be calculated.");
			throw std::runtime_error("The FIRST set of the non-terminals of this production vector is yet to be calculated.");
		};

		/**
		* Calculates FOLLOW for all non-terminals of this production vector.
		* @return true if FOLLOW is calculated, otherwise, calculates FOLLOW and then returns true.
		*/
		bool calculateFOLLOW();
		bool FOLLOWCalculated() { return this->m_CalculatedFOLLOW; };
		std::set<SymbolT> getFOLLOW(decltype(SymbolT().as.nonTerminal) nonTerminal) {

			// if FOLLOW is already calculated
			if (this->m_CalculatedFOLLOW)
				return this->FOLLOW[(size_t)nonTerminal];

			// handle the nonpresence of the FOLLOW set for this production vector
			this->m_Logger.log(LoggerInfo::ERR_MISSING_VAL, "The FOLLOW set of the non-terminals of this production vector is yet to be calculated.");
			throw std::runtime_error("The FOLLOW set of the non-terminals of this production vector is yet to be calculated.");
		};

	};

	// ALIASES
	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	using ProdVec = ProductionVector<SymbolT, SynthesizedT, ActionT>;
}

namespace m0st4fa {

	/** 1st Algorithm, assuming non-terminal E and a boolean isCalculated
	* If isCaculated, return; else procced.
	* Go through every production of E.
	* Take the FIRST set of the first grammar symbol T of that production (and cache it for possible use later).
	* Ignore that grammar symbol if it is currSymbol (this avoids lots of indirect recursion).
		* Note that this is not a problem, since in this case the FIRST set of the E is a subset of the FIRST set of T.
		* However, we are already calculating the FIRST set of E.
		* Operate in two modes, the mode in which you have a currSymbol and the in which you do not.
	* Loop and do this until the FIRST set of that grammar symbol does not contain epsilon.
		* If, while looping you encouter E or E is the first grammar symbol of the production, put the production in pending list with the index of E.
			* In case epsilon is in the FIRST set of E, we will need to rescan this production again from after index.
		* If you reach the end of the production, if the last (cached) FIRST set has epsilon, make sure it is in FIRST set of E.
	* Set isCalculated to true.
	*/

	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	bool ProductionVector<SymbolT, SynthesizedT, ActionT>::calculateFIRST()
	{

		// if FIRST is already calculated, return
		if (this->m_CalculatedFIRST) {
			this->m_Logger.logDebug("FIRST set for this production vector has already been calculated!");
			return true;
		}

		/** Algorithm
		* Loop through every production of the grammar until you cannot add another terminal to the first set of any non-terminal.
		* Variable added asserts whether we added any terminal in the last iteration to the FIRST set of any non-terminal.
		* For every production P and its head H:
			* For every grammar symbol S of the body of P:
				* If S == H, check whether FIRST(H) contains epsilon:
					* If it does, check whether we are at the end of the body of P:
						* If we are, add epsilon to FIRST(H) [Note: already added].
						* If we are not, move on to the next grammar symbol.
					* If it does not, move on to the next production.
				* If S is a terminal T, add T to FIRST(H) and move on to the next production.
					* If T is added (was not already there), set added to true.
				* If S is a non-terminal N:
					* F = FIRST(N), FE = F has epsilon?, HE = H has epsilon?
					* Add the current F to FIRST(H).
						* If any terminal was added (was not already there) set added to true.
					* If (FE && !HE) remove the epsilon from FIRST(H).
						* Note: the condition means: if F has epsilon and FIRST(H) didn't have epsilon before mergin the two FIRST sets.
					* If FE, check whether we are at the end of the body of P:
						* If we are, add epsilon to FIRST(H).
							* If it was added (was not already there) set added to true.
						* If we are not, move on to the next grammar symbol of the body of P.
				* Move on to the next production.
		*
		*/

		using SetPair = std::pair<std::set<SymbolT>, bool>;

		// resize the FIRST set to accomdate an entry for all non-terminals
		this->FIRST.resize((size_t)decltype(SymbolT().as.nonTerminal)::NT_NUM);
		bool added = false;

		this->m_Logger.logDebug("\nCALCULATING FIRST SET:\n");
		this->m_Logger.logDebug(std::format("Productions:\n {}", (std::string)*this));

		while (true) {

			// loop through every production
			for (const ProdRec& prod : this->getProdVector()) {

				// loop through every symbol of the production
				for (size_t index = 1; const auto & stackElement : prod.prodBody) {

					// if the current stack element of the production is not a grammar symbol
					if (stackElement.type != StackElementType::SET_GRAM_SYMBOL)
						// move on to the next stack element of the production
						continue;

					// if the current stack element is a grammar symbol

					// get the symbol and its first set (if any)
					const SymbolT& symbol = stackElement.as.gramSymbol;

					// continue or not?
					bool cont = _calc_FIRST_of_prod(prod, symbol, added, index);

					// increment the index of the current element
					++index;

					if (-not cont)
						break;

				}

			}

			this->m_Logger.logDebug(std::format("Is there any new terminal added in this round? {}", added ? "true" : "false"));
			if (-not added) {

				//// if we still have extra rounds
				//if (extraRound--)
				//	// reduce the number of rounds and continue to the next iteration
				//	continue;

				this->m_Logger.logDebug("Finished creating the FIRST set of all non-terminals of this grammar");


				for (size_t i = 0; const std::set<SymbolT>&set : this->FIRST) {

					// if the set is empty, continue (in this case it does not belong to a non-terminal)
					if (set.empty()) {
						i++;
						continue;
					}

					// get variable
					auto variable = (decltype(set.begin()->as.nonTerminal))i++;

					this->m_Logger.logDebug(std::format("{} => {}", stringfy(variable), stringfy(set)));
				}


				break;
			}

			added = false;
		}

		// if we reached here, that means that FIRST has been calculated
		return this->m_CalculatedFIRST = true;
	}

	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	bool ProductionVector<SymbolT, SynthesizedT, ActionT>::_calc_FIRST_of_prod(const ProdRec& prod, const SymbolT& symbol, bool& added, size_t index)
	{

		const SymbolT& head = prod.prodHead;
		size_t setIndexH = (size_t)head.as.nonTerminal;
		std::set<SymbolT>& fsetH = this->FIRST[setIndexH];

		// check whether the first set contains epsilon
		bool containsEpsilonH = fsetH.contains(SymbolT::EPSILON);

		// if the symbol is a non-terminal
		if (!symbol.isTerminal) {

			// get the first set of the symbol	
			size_t setIndexN = (size_t)symbol.as.nonTerminal;
			std::set<SymbolT>& fsetN = this->FIRST[setIndexN];

			// check whether the first set contains epsilon
			bool containsEpsilonN = fsetN.contains(SymbolT::EPSILON);
			// if the symbol is the same as head
			if (symbol == head) {
				/**
				* If S == H, check whether FIRST(H) contains epsilon:
					* If it does, check whether we are at the end of the body of P:
						* If we are, add epsilon to FIRST(H) [Note: already added].
						* If we are not, move on to the next grammar symbol.
					* If it does not, move on to the next production.
				*/

				// 1. if the first set does not contain epsilon
				if (-not containsEpsilonN)
					// move on to the next production
					return false;

				// 2. if FIRST contains epsilon

				// move on to the next grammar symbol, which will automatically move on to the next production if this is the last grammar symbol
				// Note: epsilon is already added (that's why we are here in the first place)
				return true;
			}

			// if the symbol is differnt from head
			/**
			* If S is a non-terminal N:
				* F = FIRST(N), FE = F has epsilon?, HE = H has epsilon?
				* Add the current F to FIRST(H).
					* If any terminal was added (was not already there) set added to true.
				* If (FE && !HE) remove the epsilon from FIRST(H).
					* Note: the condition means: if F has epsilon and FIRST(H) didn't have epsilon before mergin the two FIRST sets.
				* If FE, check whether we are at the end of the body of P:
					* If we are, add epsilon to FIRST(H).
						* If it was added (was not already there) set added to true.
					* If we are not, move on to the next grammar symbol of the body of P.
			*/

			if (fsetN.empty())
				return false;

			// loop through each symbol of the FIRST set of the current non-terminal
			for (const auto& symbol : fsetN) {

				// skip to the next symbol if the current is epsilon (optimize this out if you could)
				if (symbol == SymbolT::EPSILON)
					return true;

				// add the symbol to the FIRST set of the head
				// BUG: the symbol is inserted although it exists
				auto p = this->FIRST[setIndexH].insert(symbol);

				// if it was added, set the boolean
				if (p.second) {
					added = true;

					this->m_Logger.logDebug(
						std::format("Added terminal {} to the FIRST set of {}, which is now: {}",
							(std::string)symbol,
							(std::string)head,
							stringfy(this->FIRST[setIndexH])));

				}
			}

			// if the non-terminal contains epsilon
			if (containsEpsilonN) {
				// check whether we are at the end of the production
				bool atEnd = index == prod.prodBody.size();

				// if we are not at the end of the production
				if (-not atEnd)
					// continue to the next grammar symbol
					return false;

				// if we are at the end of the production

				if (-not containsEpsilonH) {
					// add epsilon to the FIRST set of the head
					auto p = this->FIRST[setIndexH].insert(SymbolT::EPSILON);

					if (p.second) {
						added = true;

						this->m_Logger.logDebug(
							std::format("Added terminal {} to the FIRST set of {}, which is now: {}",
								(std::string)SymbolT::EPSILON,
								(std::string)head,
								stringfy(this->FIRST[setIndexH])));
					}
				}

			}

			// if does not, continue to the next grammar symbol
			return false;
		}
		else {
			/**
				*If S is a terminal T, add T to FIRST(H) and move on to the next production.
					* If T is added(was not already there), set added to true.
			*/
			auto p = this->FIRST[setIndexH].insert(symbol);

			// add epsilon to the FIRST set of the head
			if (p.second) {
				added = true;

				this->m_Logger.logDebug(
					std::format("Added terminal {} to the FIRST set of {}, which is now: {}",
						(std::string)symbol,
						(std::string)head,
						stringfy(this->FIRST[setIndexH])));
			}

			// continue to the next production
			return false;
		}

		// THIS IS UNREACHABLE
		// go to the next production by default
		return false;
	}

	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	bool ProductionVector<SymbolT, SynthesizedT, ActionT>::_calc_FOLLOW_of_nonTerminal(decltype(SymbolT().as.nonTerminal) nonTerminal, size_t prodIndex, size_t variableIndex)
	{

		/**
		* Algorithm for getFOLLOWOfNonTerminal:
			* @input:
				* Non-terminal N.
				* Index I1 of the current production P within the production vector.
				* Index of this non-terminal within this production.
			* @output: Augments FOLLOW(N) using currently available information.
			* @method: For every symbol S after N in production P whose index is I2:
				* If S is a terminal, add S to FOLLOW(N) and return.
				* If S is a non-terminal, add FIRST(S) to FOLLOW(N), then check whether FOLLOW(N) contains epsilon:
					* If FOLLOW(N) contains epsilon:
						* remove epsilon.
						* Check if we are at the end of P:
							* If we are, add FOLLOW(H) to FOLLOW(S).
							* If we are not, continue to the next symbol.
					* If FOLLOW(N) doesn't contain epsilon, return.
		*/

		// head-related values
		const ProdRec& production = this->getProdVector().at(prodIndex);
		const size_t prodBdySz = production.prodBody.size();
		const SymbolT& head = production.prodHead;
		size_t headIndex = (size_t)head.as.nonTerminal;
		const std::set<SymbolT>& headFollow = this->FOLLOW.at(headIndex);

		// current symbol related values
		const size_t currSymIndex = (size_t)nonTerminal;
		std::set<SymbolT>& currSymFollow = this->FOLLOW.at(currSymIndex);
		bool added = false;

		/**
		* a lambda to copy a symbol to the follow set of the currently - being - examined non - temrinal
		* its main purpose is to be used iteratively to copy a list
		* as a side effect, this function also sets `added`
		*/ 
		auto cpyToFollow = [this, currSymIndex, nonTerminal, &currSymFollow, &added](const SymbolT& sym) {
			auto p = currSymFollow.insert(sym);

			// if a new element was added (exclude epsilon)
			if (p.second && *p.first != SymbolT::EPSILON) {
				added = true;
				this->m_Logger.logDebug(
					std::format("Added terminal {} to the FOLLOW set of {}, which is now: {}",
						(std::string)sym,
						stringfy(nonTerminal),
						stringfy(this->FOLLOW[currSymIndex])));

			}

		};


		// test if this non-terminal is the last of its production body
		bool lastSymbol = variableIndex == prodBdySz - 1;

		// if this is the last symbol of the production body
		if (lastSymbol) {
			// add FOLLOW(head) to FOLLOW(currSymbol)
			std::for_each(headFollow.begin(), headFollow.end(), cpyToFollow);
			return added;
		};
		
		for (size_t varIndex = variableIndex + 1; varIndex < prodBdySz; varIndex++) {

			// check if the current symbol we are examining in the production is a grammar symbol 
			if (production.prodBody.at(varIndex).type != StackElementType::SET_GRAM_SYMBOL) {
				// check if we are at the end of the production
				bool atEnd = varIndex + 1 == prodBdySz;

				// continue to the next symbol if we are not at the end of the production
				if (-not atEnd)
					continue;

				// if we are at the end of the production add FOLLOW(head) to FOLLOW(currSym)
				std::for_each(headFollow.begin(), headFollow.end(), cpyToFollow);

				break;
			}

			const SymbolT symbol = production.prodBody.at(varIndex).as.gramSymbol;
			size_t symIndex = (size_t)symbol.as.nonTerminal;

			// if the symbol is a terminal
			if (symbol.isTerminal) {
				auto p = this->FOLLOW.at(currSymIndex).insert(symbol);

				this->m_Logger.logDebug(
					std::format("Added terminal {} to the FOLLOW set of {}, which is now: {}",
						(std::string)symbol,
						stringfy(nonTerminal),
						stringfy(this->FOLLOW[currSymIndex])));

				// return whether a new element has been inserted
				return p.second;
			}

			// if the symbol is a non-terminal 
			// assumes that FIRST(symbol) is already calculated
			const std::set<SymbolT>& symbolFrst = this->FIRST.at(symIndex);
			std::for_each(symbolFrst.begin(), symbolFrst.end(), cpyToFollow);

			// Try erase epsilon from follow: returns 1 or 0, the number of removed elements
			// Note: this also indicates whether we had epsilon or not.
			bool containsEpsilon = currSymFollow.erase(SymbolT::EPSILON);

			// if the FOLLOW set of the curr symbol does not contain epsilon
			if (-not containsEpsilon)
				return added;

			/**
			* Check if we are at the end of P:
				* If we are, add FOLLOW(H) to FOLLOW(S).
				* If we are not, continue to the next symbol.
			*/
			// check if we are at the end of the production
			bool atEnd = varIndex + 1 == prodBdySz;

			if (-not atEnd)
				continue;

			// if we are at the end of the production add FOLLOW(head) to FOLLOW(currSym)
			std::for_each(headFollow.begin(), headFollow.end(), cpyToFollow);
			
		}

		return added;
	}

	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	bool ProductionVector<SymbolT, SynthesizedT, ActionT>::calculateFOLLOW()
	{

		// if follow is already calculated, return
		if (this->m_CalculatedFOLLOW) {
			this->m_Logger.logDebug("FOLLOW set for this production vector has already been calculated!");
			return true;
		}

		// check that FIRST is calculated before proceeding
		if (-not this->m_CalculatedFIRST) {
			this->m_Logger.log(LoggerInfo::ERR_MISSING_VAL, "FIRST set is not calculated for the production vector: FIRST set must be calculated for a production vector before proceeding to calculate the FOLLOW set for that production vector.");
			throw std::runtime_error("FIRST set is not calculated for the production vector!");
		};

		/** Algorithm
		* Loop through every production of the grammar until you cannot add another terminal to the FOLLOW set of any non-terminal.
		* Variable added asserts whether we added any terminal in the last iteration to the FOLLOW set of any non-terminal.
		* For every production P and its head H:
			* For every grammar symbol S of the body of P:
				* If S is a terminal, get to the next grammar symbol.
				* If S is a non-terminal call `getFOLLOWOfNonTerminal`.
		* Algorithm for getFOLLOWOfNonTerminal:
			* @input:
				* Non-terminal N.
				* Index I1 of the current production P within the production vector.
				* Index of this non-terminal within this production.
			* @output: Augments FOLLOW(N) using currently available information.
			* @method: For every symbol S after N in production P whose index is I2:
				* If S is a terminal, add S to FOLLOW(N) and return.
				* If S is a non-terminal, add FIRST(S) to FOLLOW(N), then check whether FOLLOW(N) contains epsilon:
					* If FOLLOW(N) contains epsilon:
						* remove epsilon.
						* Check if we are at the end of P:
							* If we are, add FOLLOW(H) to FOLLOW(S).
							* If we are not, continue to the next symbol.
					* If FOLLOW(N) doesn't contain epsilon, return.
		*/

		// make sure FOLLOW can hold all the non-terminals
		this->FOLLOW.resize((size_t)decltype(SymbolT().as.nonTerminal)::NT_NUM);

		
		bool added = false;

		this->m_Logger.logDebug("\nCALCULATING FOLLOW SET:\n");
		this->m_Logger.logDebug(std::format("Productions:\n {}", (std::string)*this));

		const ProdRec& startProd = getProdVector().at(0);
		const SymbolT& startSym = startProd.prodHead;
		size_t startHeadIndex = (size_t)startSym.as.nonTerminal;

		this->FOLLOW.at(startHeadIndex).insert(SymbolT::END_MARKER);

		while (true) {

			/**
			* Loop through every production of the grammar until you cannot add another terminal to the FOLLOW set of any non-terminal.
			* Variable added asserts whether we added any terminal in the last iteration to the FOLLOW set of any non-terminal.
			* For every production P and its head H:
				* For every grammar symbol S of the body of P:
					* If S is a terminal, get to the next grammar symbol.
					* If S is a non-terminal call `getFOLLOWOfNonTerminal`.
			*/

			for (size_t prodIndex = 0; const auto & prod : this->getProdVector()) {

				for (size_t symIndex = 0; const auto& stackElement : prod.prodBody) {

					// if the current stack element of the production is not a grammar symbol
					if (stackElement.type != StackElementType::SET_GRAM_SYMBOL) {
						// move on to the next stack element of the production
						symIndex++;
						continue;
					}

					// if the current stack element is a grammar symbol

					// get the symbol and its first set (if any)
					const SymbolT& symbol = stackElement.as.gramSymbol;

					// if the symbol is a terminal, get to the next symbol
					if (symbol.isTerminal) {
						symIndex++;
						continue;
					}

					// if the symbol is a non-terminal
					bool _added = this->_calc_FOLLOW_of_nonTerminal(symbol.as.nonTerminal, prodIndex, symIndex);

					// set added
					if (_added)
						added = true;

					// increment the index of the symbol
					symIndex++;
				}

				prodIndex++;
			}

			this->m_Logger.logDebug(std::format("Is there any new terminal added to any FOLLOW set in this round? {}", added ? "true" : "false"));
			if (-not added) {

				//// if we still have extra rounds
				//if (extraRound--)
				//	// reduce the number of rounds and continue to the next iteration
				//	continue;

				this->m_Logger.logDebug("Finished creating the FOLLOW set of all non-terminals of this grammar");

				for (size_t i = 0; const std::set<SymbolT>&set : this->FOLLOW) {

					// if the set is empty, continue (in this case it does not belong to a non-terminal)
					if (set.empty()) {
						i++;
						continue;
					}

					// get variable
					auto variable = (decltype(set.begin()->as.nonTerminal))i++;

					this->m_Logger.logDebug(std::format("{} => {}", stringfy(variable), stringfy(set)));

				}

				break;
			}

			added = false;

		}

		// if we reached here, that means that FOLLOW has been calculated
		return this->m_CalculatedFOLLOW = true;
	}
}
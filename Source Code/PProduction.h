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
		// TODO: enhance the encapsulation of this structure (turn it into a class and modify what is necessary)
		SymbolT prodHead = SymbolT{};
		std::vector<StackElement> prodBody;

		// methods
		//const SetType& calculateFIRST(VecType&, const SymbolT);
		//bool FIRSTCalculated();

		//const SetType& calculateFOLLOW(VecType&, const SymbolT);
		//bool FOLLOWCalculated();

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
		
		static GrammaticalSymbol EPSILON;

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
	GrammaticalSymbol<TerminalT, VariableT>
	GrammaticalSymbol<TerminalT, VariableT>::EPSILON = { true, {.terminal = TerminalT::T_EPSILON} };

	template <typename TerminalT, typename VariableT>
	using Symbol = GrammaticalSymbol<TerminalT, VariableT>;

	template <typename TerminalT, typename VariableT>
	std::ostream& operator<<(std::ostream& os, const Symbol<TerminalT, VariableT>& symbol) {

		return os << symbol.toString();
		
	}

	// TODO: enhance the encapsulation of this struct
	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	struct ProductionVector {
		using ProdRec = ProductionRecord<SymbolT, SynthesizedT, ActionT>;
		using VecType = std::vector<ProdRec>;
		// TODO: consider making this use terminals instead for storage efficiencey
		using SetType = std::vector<std::set<SymbolT>>;
		
		VecType vector;
		Logger m_Logger;

		/**
		* The index of the non-terminal will hold its FIRST or FOLLOW set.
		*/
		SetType FIRST {0};
		SetType FOLLOW {0};

		ProdRec& operator [] (size_t i) {
			return this->vector.at(i);
		};

		operator std::string() {
			return this->toString();
		}

		std::string toString() {

			std::string str;

			for (const auto& prod : this->vector)
				str += (std::string)prod + "\n";

			return str;
		}

		bool calculateFIRST();
		bool _FIRST_calc_prod(const ProdRec&, const SymbolT&, bool&, size_t);
		/**
		* @return (set, exists or not)
		*/
		bool FIRSTCalculated();


		bool calculateFOLLOW();
		/**
		* @return (set, exists or not)
		*/
		bool FOLLOWCalculated();


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

		this->m_Logger.logDebug(std::format("Productions:\n {}", (std::string)*this));

		while (true) {

			// loop through every production
			for (const ProdRec& prod : this->vector) {
				const SymbolT& head = prod.prodHead;
				size_t setIndexH = (size_t)head.as.nonTerminal;
				std::set<SymbolT>& fsetH = this->FIRST[setIndexH];

				// check whether the first set contains epsilon
				bool containsEpsilonH = fsetH.contains(SymbolT::EPSILON);

				// loop through every symbol of the production
				for (size_t index = 1; const auto& stackElement : prod.prodBody) {

					// if the current stack element of the production is not a grammar symbol
					if (stackElement.type != StackElementType::SET_GRAM_SYMBOL)
						// move on to the next stack element of the production
						continue;

					// if the current stack element is a grammar symbol

					// get the symbol and its first set (if any)
					const SymbolT& symbol = stackElement.as.gramSymbol;

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
								break;

							// 2. if FIRST contains epsilon

							// move on to the next grammar symbol, which will automatically move on to the next production if this is the last grammar symbol
							// Note: epsilon is already added (that's why we are here in the first place)
							continue;
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
							break;

						// loop through each symbol of the FIRST set of the current non-terminal
						for (const auto& symbol : fsetN) {

							// skip to the next symbol if the current is epsilon (optimize this out if you could)
							if (symbol == SymbolT::EPSILON)
								continue;

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
								break;

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
						break;
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
						break;
					}

					// increment the index of the current element
					++index;
				}

			}

			this->m_Logger.logDebug(std::format("Is there any new terminal added in this round? {}", added ? "true" : "false"));
			if (-not added) {

				//// if we still have extra rounds
				//if (extraRound--)
				//	// reduce the number of rounds and continue to the next iteration
				//	continue;

				this->m_Logger.logDebug("Finished creating the FIRST set of all non-terminals of this grammar");


				for (size_t i = 0; const std::set<SymbolT>& set : this->FIRST) {

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
			else
				added = false;

		}

		// if we reached here, that means that first has been calculated
		return true;
	}

	
	template<typename SymbolT, typename SynthesizedT, typename ActionT>
	bool ProductionVector<SymbolT, SynthesizedT, ActionT>::calculateFOLLOW()
	{
	}

}
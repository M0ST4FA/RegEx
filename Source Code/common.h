#pragma once
#include <string>
#include <set>
#include <type_traits>
#include <concepts>

#include "ANSI.h"
// FUNCTIONS
namespace m0st4fa {

	// STRING
	template <typename T>
	concept ConvertableToString = std::is_convertible_v<T, std::string>;
		/*requires (T obj) { stringfy(obj); } || 
		requires (T obj) { toString(obj); };*/

	template <typename T>
	// require the object to be iterable and its elements be convertable to strings
		requires requires (T iterable) {
		{*iterable.begin()} -> ConvertableToString;
	}
	std::string stringfy(const T& iterable, bool asList = true) {

		std::string separator = asList ? ", " : "\n";

		std::string temp = "{ ";

		if (iterable.empty())
			return temp+= " }";

		temp += (std::string)*iterable.begin();

		for (size_t i = 0;  const auto & element : iterable) {
			// skip the first element
			if (-not i) {
				i++;
				continue;
			}

			temp += separator + (std::string)element;
		}

		return temp+= " }";

	}

	// INTEGER

	/**
	* extract the first integer from the string and convert it into size_t
	* the integer may not necessarily be at the beginning except if the boolean is set.
	*/
	size_t toInteger(const std::string&, bool = false);
	size_t pow(size_t, size_t);

	// ITERABLE
	/**
	* Inserts elements from `from` to `to` except any element that equals `except`
	* If the type of `except` is nullptr_t, it is ignored and all elements are added
	*/
	template <typename IterableT, typename ExceptT>
	bool insertAndAssert(const IterableT& from, IterableT& to, ExceptT except) {
		bool added = false;

		for (auto element : from) {

			if (element == except)
				continue;

			auto p = to.insert(element);

			if (p.second)
				added = true;
		}

		return added;
	};

	template <typename IterableT>
	bool insertAndAssert(const IterableT& from, IterableT& to) {
		bool added = false;

		for (auto element : from) {

			auto p = to.insert(element);

			if (p.second)
				added = true;

		}

		return added;
	};
	
	template <typename ElementT, typename IterableT>
	bool isIn(const ElementT element, const IterableT& iterable) {
		
		for (const ElementT& e : iterable)
			if (e == element)
				return true;
		
		return false;
	}

	/**
	* works with iterables that have method `contains`
	*/
	template<typename IterableT>
		requires requires (IterableT a) {
		a.contains(*a.begin());
	}
	bool operator==(const IterableT& lhs, const IterableT& rhs) {

		// if they do not have the same size, they are not coequal
		if (lhs.size() != rhs.size())
			return false;

		// if they have the same number of elements

		// if, for every element `a` in `lhs`, `a` is also an element
		// of `rhs`, then they are coequal

		for (const auto& lhsElement : lhs) {
			// check whether `rhs` contains that element
			if (rhs.contains(lhsElement))
				continue;

			// if `rhs` does not contain that element
			return false;
		}
		
		return true;
	};

	/**
	* works with iterables that do not have method `contains`, but that have method `at`
	*/
	template<typename IterableT> 
		requires requires (IterableT a) {
		a.at(0);
	}
	bool operator==(const IterableT& lhs, const IterableT& rhs) {

		const size_t lhsSize = lhs.size();
		const size_t rhsSize = rhs.size();

		// if they have different sizes, they are not equal
		if (lhsSize != rhsSize)
			return false;

		// if there exists at least a single element in `lhs`
		// that does not exist in `rhs`, in order, they are not equal
		for (size_t i = 0; i < lhsSize; i++) {
			const auto& lhsElement = lhs.at(i);
			const auto& rhsElement = rhs.at(i);

			// if the current elements in each prod body are not equal,
			// the two productions are not equal
			if (lhsElement != rhsElement)
				return false;
		}

		// if we reach here, it means the two iterables are not `not-equal`
		// hence they must be equal
		return true;
	}

	// RANGES
	template<typename T>
		requires requires(T a, T b) {
		a < b;
		a > b;
		}
	inline bool withinRange(T element, T b1, T b2, bool inclusive = false) {
		if (inclusive)
			return (element >= b1 && element <= b2);

		return (element > b1 && element < b2);
	};

}


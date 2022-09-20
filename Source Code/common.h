#pragma once
#include <string>
#include <set>
#include <type_traits>

// FUNCTIONS
namespace m0st4fa {

	template <typename T>
	concept ConvertableToString = std::is_convertible_v<T, std::string>;
		/*requires (T obj) { stringfy(obj); } || 
		requires (T obj) { toString(obj); };*/

	template <typename T>
	// require the object to be iterable and its elements be convertable to strings
		requires requires (T iterable) {
		{*iterable.begin()} -> ConvertableToString;
	}
	std::string stringfy(const T& iterable) {

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

			temp += ", " + (std::string)element;
		}

		return temp+= " }";

	}

	/**
	* Inserts elements from `from` to `to` except any element that equals `except`
	* If the type of `except` is nullptr_t, it is ignored and all elements are added
	*/
	template <typename IterableT, typename ExceptT>
	bool insertAndAssert(const IterableT& from, IterableT to, ExceptT except) {
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
	bool insertAndAssert(const IterableT& from, IterableT to) {
		bool added = false;

		for (auto element : from) {

			auto p = to.insert(element);

			if (p.second)
				added = true;

		}

		return added;
	};

}


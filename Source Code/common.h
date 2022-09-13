#pragma once
#include <string>
#include <set>
#include <type_traits>

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


}

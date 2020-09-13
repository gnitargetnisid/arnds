#pragma once
#pragma once

#include <sstream>
#include "first_order_logic/propositional_logic.h"

namespace ND
{
	typedef std::uint64_t ID;

	//Tuple wrapper so we can generate an std::tuple<T, T, T...> by using Tuple<N, T>
	template<size_t N, typename T>
	struct Tuple_Ext
	{
		template <typename... Args>
		using type = typename Tuple_Ext<N - 1, T>::template type<T, Args...>;
	};
	
	template <typename T>
	struct Tuple_Ext<0, T>
	{
		template<typename... Args>
		using type = std::tuple<Args...>;
	};

	template <size_t N, typename T>
	using Tuple = typename Tuple_Ext<N, T>::template type<>;

	template <size_t N, typename T, size_t i = 0>
	struct ConvertTupleToVector
	{
		static std::vector<T> Convert(const Tuple<N, T>& tuple)
		{
			std::vector<T> vec;
			vec.push_back(std::get<i>(tuple));
			std::vector<T> restVec = ConvertTupleToVector<N, T, i + 1>::Convert(tuple);
			vec.insert(vec.end(), restVec.begin(), restVec.end());
			return vec;
		}
	};

	template<size_t N, typename T>
	struct ConvertTupleToVector<N, T, N>
	{
		static std::vector<T> Convert(const Tuple<N, T>& tuple) { return std::vector<T>(); }
	};

	template <typename T>
	bool ConvertFromString(const std::string& str, T& dest)
	{
		std::istringstream stream(str);
		stream >> dest;
		return !stream.fail();
	}
}
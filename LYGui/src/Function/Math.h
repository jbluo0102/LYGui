#pragma once

#include <random>

namespace FMath {
	/** Returns random number in range **/
	template<typename T>
	T GetRandom(const T& Min, const T& Max)
	{
		std::random_device rd;
		std::mt19937 mt(rd());
		std::uniform_int_distribution<T> dist(Min, Max);

		return dist(mt);
	}

	template<typename T>
	T Clamp(const T& Value, const T& Min, const T& Max)
	{
		return Value > Max ? Max : Value < Min ? Min : Value;
	}
}
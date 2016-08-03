#pragma once
#include <random>

//! returns a floating point number between 0 and 1
inline float randFloat()
{
	static std::default_random_engine e;
	static std::uniform_real_distribution<float> d;
	return d(e);
}

//! template function for linearly interpolating between two values of a given type
template<typename T>
inline T lerp(const T &lhs, const T &rhs, float aT)
{
	return (1.0f - aT) * lhs + aT * rhs;
}
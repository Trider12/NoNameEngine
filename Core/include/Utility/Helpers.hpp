#pragma once

#include <cmath>
#include <SFML/System/Vector2.hpp>

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

class VectorHelper final
{
public:
	constexpr static const float epsilon = 0.00001f;

	template <Arithmetic T>
	inline static auto abs(const sf::Vector2<T>& v)
	{
		return sf::Vector2<T>(std::abs(v.x), std::abs(v.y));
	}

	template <Arithmetic T>
	inline static auto length(const sf::Vector2<T>& v)
	{
		return std::sqrt(lengthSqr(v));
	}

	template <Arithmetic T>
	inline static auto lengthSqr(const sf::Vector2<T>& v)
	{
		return dot(v, v);
	}

	template <Arithmetic T>
	inline static auto normalized(const sf::Vector2<T>& v)
	{
		return v / length(v);
	}

	template <Arithmetic T>
	inline static auto normalize(sf::Vector2<T>& v)
	{
		return v /= length(v);
	}

	template <Arithmetic T>
	inline static auto dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
	{
		return a.x * b.x + a.y * b.y;
	}

	template <Arithmetic T>
	inline static auto distance(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
	{
		return length(a - b);
	}

	template <Arithmetic T>
	inline static auto distanceSqr(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
	{
		return lengthSqr(a - b);
	}

	template <Arithmetic T>
	inline static auto areEqualApprox(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
	{
		return a == b;
	}

	template <>
	inline static auto areEqualApprox<float>(const sf::Vector2<float>& a, const sf::Vector2<float>& b)
	{
		return (std::fabsf(a.x - b.x) < epsilon) && (std::fabsf(a.y - b.y) < epsilon);
	}
};
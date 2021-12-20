#pragma once

#include <cmath>
#include <SFML/System/Vector2.hpp>

#define EPSILON 0.00001

template <typename T>
concept Arithmetic = std::is_arithmetic_v<T>;

template <Arithmetic T>
inline auto dot(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
	return a.x * b.x + a.y * b.y;
}

template <Arithmetic T>
inline auto lengthSqr(const sf::Vector2<T>& v)
{
	return dot(v, v);
}

template <Arithmetic T>
inline auto length(const sf::Vector2<T>& v)
{
	return std::sqrt(lengthSqr(v));
}

template <Arithmetic T>
inline auto normalized(const sf::Vector2<T>& v)
{
	return v / length(v);
}

template <Arithmetic T>
inline auto normalize(sf::Vector2<T>& v)
{
	return v /= length(v);
}

template <Arithmetic T>
inline auto distance(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
	return length(a - b);
}

template <Arithmetic T>
inline auto areEqualApprox(const sf::Vector2<T>& a, const sf::Vector2<T>& b)
{
	return a == b;
}

template <>
inline auto areEqualApprox<float>(const sf::Vector2<float>& a, const sf::Vector2<float>& b)
{
	return (std::fabsf(a.x - b.x) < EPSILON) && (std::fabsf(a.y - b.y) < EPSILON);
}
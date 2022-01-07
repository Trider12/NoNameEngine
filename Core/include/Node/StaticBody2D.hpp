#pragma once

#include <SFML/System/Vector2.hpp>

#include "Node2D.hpp"

class StaticBody2D : public Node2D
{
public:
	StaticBody2D();
	StaticBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions);
	StaticBody2D(const sf::Vector2f& position, float radius);
};
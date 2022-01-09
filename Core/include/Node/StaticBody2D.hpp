#pragma once

#include "Node/PhysicsBody2D.hpp"

class StaticBody2D : public PhysicsBody2D
{
public:
	StaticBody2D();
	StaticBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions);
	StaticBody2D(const sf::Vector2f& position, float radius);
};
#pragma once

#include "Node/Node2D.hpp"

#include <SFML/System/Vector2.hpp>

class PhysicsBody2D : public Node2D
{
public:
	PhysicsBody2D() : Node2D() {}

	PhysicsBody2D(const sf::Vector2f& translation) : Node2D(translation) {}

	virtual void onCollision(sf::Vector2f normal)
	{
	}
};
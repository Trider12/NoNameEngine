#pragma once

#include <SFML/System/Vector2.hpp>

#include "Node/Node2D.hpp"

class KinematicBody2D : public Node2D
{
public:
	KinematicBody2D();
	KinematicBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions);
	KinematicBody2D(const sf::Vector2f& position, float radius);
};
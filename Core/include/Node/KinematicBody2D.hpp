#pragma once

#include "Node/PhysicsBody2D.hpp"

class KinematicBody2D : public PhysicsBody2D
{
public:
	KinematicBody2D();
	KinematicBody2D(const sf::Vector2f& position, const sf::Vector2f& dimensions, PhysicsBody2D::CollisionShapeType collisionShapeType = PhysicsBody2D::CollisionShapeType::AABB);
	KinematicBody2D(const sf::Vector2f& position, float radius);
};
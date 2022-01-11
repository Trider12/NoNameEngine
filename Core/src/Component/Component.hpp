#pragma once

#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "Node/PhysicsBody2D.hpp"

struct Component
{
};

template <typename T>
concept DerivedComponent = std::is_base_of_v<Component, T>;

struct TransformComponent : public Component
{
	sf::Transform globalTransform;
	sf::Transform deferredTransform;
	Node2D* node2D;
};

struct TriangulatedPrimitiveComponent : public Component
{
	sf::Vector2f* trianglePoints = nullptr;
	sf::Color color = sf::Color::White;
	uint64_t trianglePointsCount = 0u;
	uint64_t bufferOffset = 0u;
};

struct CollisionComponent : public Component
{
	enum class CollisionType : uint8_t
	{
		Static = 0,
		Kinematic = 1,
		//Rigid = 2 // TODO
	};

	enum class CollisionShapeType : uint8_t
	{
		AABB = 0,
		Circle = 1,
		//Rectangle = 2 // TODO
	};

	struct CollisionShape
	{
		union
		{
			sf::FloatRect rect;

			struct
			{
				sf::Vector2f center;
				float radius;
			};
		};
	};

	CollisionComponent()
		: collisionType{ CollisionType::Static }
		, collisionShapeType{ CollisionShapeType::AABB }
		, collisionShape{ CollisionShape { sf::FloatRect() } }
	{
	}

	CollisionComponent(CollisionType type, CollisionShapeType shapeType)
		: collisionType{ type }
		, collisionShapeType{ shapeType }
		, collisionShape{ }
	{
		switch (collisionShapeType)
		{
			case CollisionComponent::CollisionShapeType::AABB:
			{
				collisionShape.rect = sf::FloatRect(0.f, 0.f, 100.f, 100.f);
				break;
			}
			case CollisionComponent::CollisionShapeType::Circle:
			{
				collisionShape.center = {};
				collisionShape.radius = 10.f;
				break;
			}
			default:
			{
				collisionShape.rect = sf::FloatRect(0.f, 0.f, 100.f, 100.f);
				break;
			}
		}
	}

	CollisionType collisionType;
	CollisionShapeType collisionShapeType;
	CollisionShape collisionShape;
	PhysicsBody2D* physicsBody = nullptr;
};
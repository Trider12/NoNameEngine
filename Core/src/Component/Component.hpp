#pragma once

#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <SFML/Graphics/Rect.hpp>

enum class ComponentType
{
	Transform2D,
	ColorRect,
	Texture,
	Collision2D,
};

struct Component
{
};

template <typename T>
concept DerivedComponent = std::is_base_of_v<Component, T>;

struct TransformComponent : public Component
{
	sf::Transform globalTransform;
	sf::Transform deferredTransform;
};

struct QuadVerticesComponent : public Component
{
	QuadVerticesComponent() {}
	QuadVerticesComponent(const sf::Vertex& v1, const sf::Vertex& v2, const sf::Vertex& v3, const sf::Vertex& v4) : vertices{ v1, v2, v3, v4 } {}

	sf::Vertex vertices[4];
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
};
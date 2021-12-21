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
	CollisionComponent()
		: collisionShapeType{ CollisionShapeType::AABB }
		, collisionShape{ CollisionShape { sf::FloatRect() } }
	{
	}

	enum CollisionShapeType
	{
		AABB = 0,
		//Rectangle = 1, // TODO
		Circle = 2
	} collisionShapeType;

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
	} collisionShape;
};
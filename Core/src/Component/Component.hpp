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
	sf::Transform globalTransform = sf::Transform::Identity;
	sf::Transform deferredTransform = sf::Transform::Identity;
};

struct QuadVerticesComponent : public Component
{
	sf::Vertex vertices[4];
};

struct CollisionComponent : public Component
{
	enum CollisionShapeType
	{
		Rectangle, Circle
	} collisionShapeType;

	struct CollisionShape
	{
		sf::Transform transform;
		union
		{
			struct
			{
				float width, height;
			};

			float radius;
		};
	} collisionShape;
};
#pragma once

#include "SFML/Graphics/Transform.hpp"
#include "SFML/Graphics/Rect.hpp"

struct Component
{
};

struct TransformComponent : public  Component
{
	sf::Transform localTransform = sf::Transform::Identity;
	sf::Transform globalTransform = sf::Transform::Identity;
	sf::Transform deferredTransform = sf::Transform::Identity;
};

struct CollisionComponent : public  Component
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

struct Graphics2DComponent : public  Component
{
	sf::Transform transform;
};
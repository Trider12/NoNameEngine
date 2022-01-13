#include "System/PhysicsSystem.hpp"
#include "System/SystemManager.hpp"
#include "Utility/Helpers.hpp"
#include "Core/Debug.hpp"

#if DEBUG
#include "Core/Locator.hpp"
#endif // DEBUG

namespace
{
	const uint8_t initialNumberOfComponents = 32;

	bool preventCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation);
}

PhysicsSystem::PhysicsSystem(SystemManager& manager)
	: System(manager)
	, _transformComponents{ ComponentArray<TransformComponent>(initialNumberOfComponents) }
	, _collisionComponents{ ComponentArray<CollisionComponent>(initialNumberOfComponents) }
{
}

void PhysicsSystem::update(float delta)
{
	std::scoped_lock lock(mutex);

	_transformComponents.update();
	_collisionComponents.update();

	sf::Vector2f translation;

	for (auto i = 0; i < _collisionComponents.activeCount(); ++i)
	{
		auto& c1 = _collisionComponents[i];
		auto& t1 = _transformComponents[_collisionComponents.getNodeId(i)];

		for (auto j = i + 1; j < _collisionComponents.activeCount(); ++j)
		{
			auto& c2 = _collisionComponents[j];
			auto& t2 = _transformComponents[_collisionComponents.getNodeId(j)];

			translation.x = 0.f;
			translation.y = 0.f;

			if (preventCollision(c1, c2, t1, t2, translation))
			{
				if (c1.collisionType == CollisionComponent::CollisionType::Kinematic)
				{
					assert(t1.node2D != nullptr);

					t1.node2D->translateDeferred(translation);
				}
				else
				{
					assert(t2.node2D != nullptr);

					t2.node2D->translateDeferred(-translation);
				}

				VectorHelper::normalize(translation);

				if (c1.physicsBody != nullptr)
				{
					c1.physicsBody->onCollision(translation);
				}

				if (c2.physicsBody != nullptr)
				{
					c2.physicsBody->onCollision(translation);
				}
			}
		}
	}

	for (auto& transformComponent : _transformComponents)
	{
		transformComponent.globalTransform *= transformComponent.deferredTransform;
		transformComponent.deferredTransform = sf::Transform::Identity;
	}

	_systemManager.getSystem<RenderSystem>().setTransformsDirty();
}

void PhysicsSystem::transformDeferred(const Node2D& node, sf::Transform transform)
{
	_transformComponents.getComponent(node.getId()).deferredTransform *= transform;
}

namespace
{
	bool rectsAreNotOverlapping(const sf::FloatRect& rect1, const sf::FloatRect& rect2)
	{
		return rect1.left > rect2.left + rect2.width || rect2.left > rect1.left + rect1.width ||
			rect1.top > rect2.top + rect2.height || rect2.top > rect1.top + rect1.height;
	}

	bool pureStaticCheck(const CollisionComponent& c1, const CollisionComponent& c2)
	{
		if (c1.collisionType == CollisionComponent::CollisionType::Static && c2.collisionType == CollisionComponent::CollisionType::Static)
		{
			Debug::Warning("pure static collisions ignored!");
			return true;
		}

		return false;
	}

	bool preventAABBxAABBCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		auto rect1 = (t1.globalTransform * t1.deferredTransform).transformRect(c1.collisionShape.rect);
		auto rect2 = (t2.globalTransform * t2.deferredTransform).transformRect(c2.collisionShape.rect);

		if (rectsAreNotOverlapping(rect1, rect2))
		{
			return false;
		}

		auto dx = (rect1.left + rect1.width / 2) - (rect2.left + rect2.width / 2);
		auto dy = (rect1.top + rect1.height / 2) - (rect2.top + rect2.height / 2);
		auto minDx = rect1.width / 2 + rect2.width / 2;
		auto minDy = rect1.height / 2 + rect2.height / 2;

		if (std::fabsf(dx) >= minDx || std::fabsf(dy) >= minDy || pureStaticCheck(c1, c2))
		{
			return false;
		}

		auto depthX = std::copysignf(minDx, dx) - dx;
		auto depthY = std::copysignf(minDy, dy) - dy;

		translation = std::fabsf(depthX) <= std::fabsf(depthY) ? sf::Vector2f(depthX, 0.f) : sf::Vector2f(0.f, depthY);

		return true;
	}

	bool preventCirclexCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		auto center1 = (t1.globalTransform * t1.deferredTransform).transformPoint(c1.collisionShape.center);
		auto center2 = (t2.globalTransform * t2.deferredTransform).transformPoint(c2.collisionShape.center);
		auto radius1 = c1.collisionShape.radius;
		auto radius2 = c2.collisionShape.radius;

		if (rectsAreNotOverlapping({ center1.x - radius1, center1.y - radius1, radius1 * 2.f, radius1 * 2.f }, { center2.x - radius2, center2.y - radius2, radius2 * 2.f, radius2 * 2.f }))
		{
			return false;
		}

		auto deltaCenter = center1 - center2;
		auto distSqr = VectorHelper::lengthSqr(deltaCenter);

		if (distSqr - std::powf(radius1 + radius2, 2.f) >= 0.f || pureStaticCheck(c1, c2))
		{
			return false;
		}

		translation = (radius1 + radius2 - std::sqrtf(distSqr)) * VectorHelper::normalized(deltaCenter);

		return true;
	}

	bool preventAABBxCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, sf::Vector2f& translation, sf::FloatRect rect, sf::Vector2f circleCenter, float circleRadius)
	{
		auto circleRect = sf::FloatRect(circleCenter.x - circleRadius, circleCenter.y - circleRadius, circleRadius * 2.f, circleRadius * 2.f);

		if (rectsAreNotOverlapping(rect, circleRect))
		{
			return false;
		}

		auto halfDims = sf::Vector2f(rect.width / 2.f, rect.height / 2.f);
		auto rectCenter = sf::Vector2f(rect.left + halfDims.x, rect.top + halfDims.y);
		auto circleToRectDirection = VectorHelper::normalized(rectCenter - circleCenter);
		auto circumferencePointLookingAtRect = circleCenter + circleRadius * circleToRectDirection;
		auto circumferencePointNormalToRect = circleCenter + (std::fabsf(circleToRectDirection.x) < std::fabsf(circleToRectDirection.y) ?
			sf::Vector2f(std::copysignf(circleRadius, circleToRectDirection.x), 0.f) :
			sf::Vector2f(0.f, std::copysignf(circleRadius, circleToRectDirection.y)));

		// TODO: I have a big feeling this can be simplified

		if (!rect.contains(circumferencePointLookingAtRect) && !rect.contains(circumferencePointNormalToRect) || pureStaticCheck(c1, c2))
		{
			return false;
		}

		auto diagonalTangent = rect.height / rect.width;
		auto tangent = std::fabsf(circleToRectDirection.y / circleToRectDirection.x);

		sf::Vector2f intersection;

		if (tangent > diagonalTangent)
		{
			if (circleCenter.x > rect.left && circleCenter.x < rect.left + rect.width)
			{
				intersection.x = circleCenter.x;
				intersection.y = circleCenter.y + std::copysignf(circleRadius, circleToRectDirection.y);
			}
			else
			{
				intersection.x = rectCenter.x - std::copysignf(halfDims.x, circleToRectDirection.x);
				intersection.y = circleCenter.y + std::copysignf(std::sqrtf(circleRadius * circleRadius - std::powf(intersection.x - circleCenter.x, 2.f)), circleToRectDirection.y);
			}

			translation.y = std::copysignf(halfDims.y, circleToRectDirection.y) + intersection.y - rectCenter.y;
		}
		else
		{
			if (circleCenter.y > rect.top && circleCenter.y < rect.top + rect.height)
			{
				intersection.y = circleCenter.y;
				intersection.x = circleCenter.x + std::copysignf(circleRadius, circleToRectDirection.x);
			}
			else
			{
				intersection.y = rectCenter.y - std::copysignf(halfDims.y, circleToRectDirection.y);
				intersection.x = circleCenter.x + std::copysignf(std::sqrtf(circleRadius * circleRadius - std::powf(intersection.y - circleCenter.y, 2.f)), circleToRectDirection.x);
			}

			translation.x = std::copysignf(halfDims.x, circleToRectDirection.x) + intersection.x - rectCenter.x;
		}

		return true;
	}

	bool preventAABBxCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		auto rect = (t1.globalTransform * t1.deferredTransform).transformRect(c1.collisionShape.rect);
		auto circleCenter = (t2.globalTransform * t2.deferredTransform).transformPoint(c2.collisionShape.center);
		auto circleRadius = c2.collisionShape.radius;

		return preventAABBxCircleCollision(c1, c2, translation, rect, circleCenter, circleRadius);
	}

	bool preventRectanglexCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		return false; // TODO
	}

	bool preventAABBxRectangleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		return false; // TODO
	}

	bool preventRectanglexRectangleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		return false; // TODO
	}

	bool preventCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2, sf::Vector2f& translation)
	{
		switch (c1.collisionShapeType)
		{
			case PhysicsBody2D::CollisionShapeType::AABB:
			{
				switch (c2.collisionShapeType)
				{
					case PhysicsBody2D::CollisionShapeType::AABB:
						return preventAABBxAABBCollision(c1, c2, t1, t2, translation);
						break;
					case PhysicsBody2D::CollisionShapeType::Circle:
						return preventAABBxCircleCollision(c1, c2, t1, t2, translation);
						break;
					case PhysicsBody2D::CollisionShapeType::Rectangle:
						return preventAABBxRectangleCollision(c1, c2, t1, t2, translation);
						break;
				}

				break;
			}
			case PhysicsBody2D::CollisionShapeType::Circle:
			{
				switch (c2.collisionShapeType)
				{
					case PhysicsBody2D::CollisionShapeType::AABB:
						return preventAABBxCircleCollision(c2, c1, t2, t1, translation);
						break;
					case PhysicsBody2D::CollisionShapeType::Circle:
						return preventCirclexCircleCollision(c1, c2, t1, t2, translation);
						break;
					case PhysicsBody2D::CollisionShapeType::Rectangle:
						return preventRectanglexCircleCollision(c2, c1, t2, t1, translation);
						break;
				}

				break;
			}
			case PhysicsBody2D::CollisionShapeType::Rectangle:
			{
				switch (c2.collisionShapeType)
				{
					case PhysicsBody2D::CollisionShapeType::AABB:
						return preventAABBxRectangleCollision(c2, c1, t2, t1, translation);
						break;
					case PhysicsBody2D::CollisionShapeType::Circle:
						return preventRectanglexCircleCollision(c1, c2, t1, t2, translation);
						break;
					case PhysicsBody2D::CollisionShapeType::Rectangle:
						return preventRectanglexRectangleCollision(c1, c2, t1, t2, translation);
						break;
				}

				break;
			}
		}

		return false;
	}
}
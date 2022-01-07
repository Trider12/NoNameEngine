#include "System/PhysicsSystem.hpp"
#include "System/SystemManager.hpp"
#include "Core/Debug.hpp"
#include "Utility/Helpers.hpp"

namespace
{
	const uint8_t initialNumberOfComponents = 32;

	void preventCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2);
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

	for (auto i = 0; i < _collisionComponents.activeCount(); ++i)
	{
		auto& c1 = _collisionComponents[i];
		auto& t1 = _transformComponents[_collisionComponents.getNodeId(i)];

		for (auto j = i + 1; j < _collisionComponents.activeCount(); ++j)
		{
			auto& c2 = _collisionComponents[j];
			auto& t2 = _transformComponents[_collisionComponents.getNodeId(j)];

			preventCollision(c1, c2, t1, t2);
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

	bool pureKinematicCheck(const CollisionComponent& c1, const CollisionComponent& c2)
	{
		if (c1.collisionType == CollisionComponent::CollisionType::Kinematic && c2.collisionType == CollisionComponent::CollisionType::Kinematic)
		{
			Debug::Warning("pure kinematic collisions ignored!");
			return true;
		}

		return false;
	}

	void preventAABBxAABBCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		auto rect1 = (t1.globalTransform * t1.deferredTransform).transformRect(c1.collisionShape.rect);
		auto rect2 = (t2.globalTransform * t2.deferredTransform).transformRect(c2.collisionShape.rect);

		if (rectsAreNotOverlapping(rect1, rect2))
		{
			return;
		}

		auto dx = (rect1.left + rect1.width / 2) - (rect2.left + rect2.width / 2);
		auto dy = (rect1.top + rect1.height / 2) - (rect2.top + rect2.height / 2);
		auto minDx = rect1.width / 2 + rect2.width / 2;
		auto minDy = rect1.height / 2 + rect2.height / 2;

		if (std::fabsf(dx) >= minDx || std::fabsf(dy) >= minDy)
		{
			return;
		}

		if (pureStaticCheck(c1, c2) || pureKinematicCheck(c1, c2))
		{
			return;
		}

		auto depthX = std::copysignf(minDx, dx) - dx;
		auto depthY = std::copysignf(minDy, dy) - dy;

		auto translation = std::fabsf(depthX) <= std::fabsf(depthY) ? sf::Vector2f(depthX, 0.f) : sf::Vector2f(0.f, depthY);
		auto fixTransform = sf::Transform::Identity;

		if (c1.collisionType == CollisionComponent::CollisionType::Kinematic)
		{
			fixTransform.translate(translation);
			t1.deferredTransform *= fixTransform;
		}
		else
		{
			fixTransform.translate(-translation);
			t2.deferredTransform *= fixTransform;
		}
	}

	void preventCirclexCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		auto center1 = (t1.globalTransform * t1.deferredTransform).transformPoint(c1.collisionShape.center);
		auto center2 = (t2.globalTransform * t2.deferredTransform).transformPoint(c2.collisionShape.center);
		auto radius1 = c1.collisionShape.radius;
		auto radius2 = c2.collisionShape.radius;

		if (rectsAreNotOverlapping({ center1.x - radius1, center1.y - radius1, radius1 * 2.f, radius1 * 2.f }, { center2.x - radius2, center2.y - radius2, radius2 * 2.f, radius2 * 2.f }))
		{
			return;
		}

		auto direction = center1 - center2;
		auto distSqr = VectorHelper::lengthSqr(direction);

		if (distSqr - std::powf(radius1 + radius2, 2.f) >= 0.f)
		{
			return;
		}

		if (pureStaticCheck(c1, c2) || pureKinematicCheck(c1, c2))
		{
			return;
		}

		auto translation = (radius1 + radius2 - std::sqrtf(distSqr)) * VectorHelper::normalized(direction);
		auto fixTransform = sf::Transform::Identity;

		if (c1.collisionType == CollisionComponent::CollisionType::Kinematic)
		{
			fixTransform.translate(translation);
			t1.deferredTransform *= fixTransform;
		}
		else
		{
			fixTransform.translate(-translation);
			t2.deferredTransform *= fixTransform;
		}
	}

	void preventAABBxCircleCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		// TODO
	}

	void preventCollision(const CollisionComponent& c1, const CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		if (c1.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
		{
			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
			{
				preventAABBxAABBCollision(c1, c2, t1, t2);
			}

			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::Circle)
			{
				preventAABBxCircleCollision(c1, c2, t1, t2);
			}
		}

		if (c1.collisionShapeType == CollisionComponent::CollisionShapeType::Circle)
		{
			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::Circle)
			{
				preventCirclexCircleCollision(c1, c2, t1, t2);
			}

			if (c2.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
			{
				preventAABBxCircleCollision(c2, c1, t2, t1);
			}
		}
	}
}
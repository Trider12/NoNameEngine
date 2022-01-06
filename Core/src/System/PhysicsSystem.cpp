#include "System/PhysicsSystem.hpp"
#include "System/SystemManager.hpp"

namespace
{
	const uint8_t initialNumberOfComponents = 32;

	void preventAABBCollision(CollisionComponent& c1, CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2);
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

			if (c1.collisionShapeType == CollisionComponent::CollisionShapeType::AABB && c2.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
			{
				preventAABBCollision(c1, c2, t1, t2);
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
	void preventAABBCollision(CollisionComponent& c1, CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		auto rect1 = (t1.globalTransform * t1.deferredTransform).transformRect(c1.collisionShape.rect);
		auto rect2 = (t2.globalTransform * t2.deferredTransform).transformRect(c2.collisionShape.rect);

		if (rect1.left > rect2.left + rect2.width || rect2.left > rect1.left + rect1.width ||
			rect1.top > rect2.top + rect2.height || rect2.top > rect1.top + rect1.height)
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

		if (c1.collisionType == CollisionComponent::CollisionType::Static && c2.collisionType == CollisionComponent::CollisionType::Static)
		{
#ifdef _DEBUG
			fprintf(stderr, "Warning: pure static collisions ignored!\n");
#endif // _DEBUG
			return;
		}

		if (c1.collisionType == CollisionComponent::CollisionType::Kinematic && c2.collisionType == CollisionComponent::CollisionType::Kinematic)
		{
#ifdef _DEBUG
			fprintf(stderr, "Warning: pure kinematic collisions ignored!\n");
#endif // _DEBUG
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
}
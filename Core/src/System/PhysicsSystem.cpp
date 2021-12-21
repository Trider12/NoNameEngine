#include "System/PhysicsSystem.hpp"
#include "System/SystemManager.hpp"

namespace
{
	const uint8_t initialNumberOfComponents = 32;

	void resolveAABBCollision(CollisionComponent& c1, CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2);
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
		for (auto j = i + 1; j < _collisionComponents.activeCount(); ++j)
		{
			auto& c1 = _collisionComponents[i];
			auto& c2 = _collisionComponents[j];
			auto& t1 = _transformComponents[_collisionComponents.getNodeId(i)];
			auto& t2 = _transformComponents[_collisionComponents.getNodeId(j)];

			if (c1.collisionShapeType == CollisionComponent::CollisionShapeType::AABB && c2.collisionShapeType == CollisionComponent::CollisionShapeType::AABB)
			{
				resolveAABBCollision(c1, c2, t1, t2);
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
	_transformComponents.getComponent(node).deferredTransform *= transform;
}

namespace
{
	void resolveAABBCollision(CollisionComponent& c1, CollisionComponent& c2, TransformComponent& t1, TransformComponent& t2)
	{
		// TODO
	}
}
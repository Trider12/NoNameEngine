#include "PhysicsSystem.hpp"
#include "SystemManager.hpp"

namespace
{
	const uint8_t initialNumberOfComponents = 64;
}

PhysicsSystem::PhysicsSystem(SystemManager& manager)
	: System(manager)
	, _transformComponents{ ComponentArray<TransformComponent>(initialNumberOfComponents) }
{
}

void PhysicsSystem::update(float delta)
{
	std::scoped_lock lock(mutex);

	for (auto& transformComponent : _transformComponents)
	{
		transformComponent.globalTransform *= transformComponent.deferredTransform;
		transformComponent.deferredTransform = sf::Transform::Identity;
	}
}

void PhysicsSystem::transformDeferred(const Node2D& node, sf::Transform transform)
{
	_transformComponents.getComponent(node).deferredTransform *= transform;
}
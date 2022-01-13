#pragma once

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"

class PhysicsSystem : public System
{
public:
	void update(float delta);

	template <DerivedComponent T>
	void addComponent(const Node2D& node, T component);

	template <DerivedComponent T>
	void removeComponent(const Node2D& node);

	template <DerivedComponent T>
	T getComponent(const Node2D& node);

	template <DerivedComponent T>
	T getComponent(uint64_t nodeId);

	void transformDeferred(const Node2D& node, sf::Transform transform);

private:
	PhysicsSystem(SystemManager& manager);

	ComponentArray<TransformComponent> _transformComponents;
	ComponentArray<CollisionComponent> _collisionComponents;

	friend class SystemManager;
	friend class RenderSystem;
};

template <DerivedComponent T>
void PhysicsSystem::addComponent(const Node2D& node, T component)
{
	if constexpr (std::is_same_v<T, TransformComponent>)
	{
		_transformComponents.addComponent(node.getId(), component);
	}
	else if constexpr (std::is_same_v<T, CollisionComponent>)
	{
		_collisionComponents.addComponent(node.getId(), component);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
void PhysicsSystem::removeComponent(const Node2D& node)
{
	if constexpr (std::is_same_v<T, TransformComponent>)
	{
		_transformComponents.remove(node.getId());
	}
	else if constexpr (std::is_same_v<T, CollisionComponent>)
	{
		_collisionComponents.remove(node.getId());
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
T PhysicsSystem::getComponent(const Node2D& node)
{
	return getComponent(node.getId());
}

template <DerivedComponent T>
T PhysicsSystem::getComponent(uint64_t nodeId)
{
	if constexpr (std::is_same_v<T, TransformComponent>)
	{
		return _transformComponents.getComponent(nodeId);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

static_assert (DerivedSystem<PhysicsSystem>);
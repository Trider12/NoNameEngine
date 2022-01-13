#pragma once

#include "System/RenderSystem.hpp"
#include "System/PhysicsSystem.hpp"
#include "Node/Node2D.hpp"

class Node2D;

class SystemManager
{
public:
	template <DerivedSystem T>
	T& getSystem();

	// shortcuts

	template <DerivedSystem T>
	void update(float delta);

	template <DerivedComponent T>
	void addComponent(const Node2D& node, T component);

	template <DerivedComponent T>
	void removeComponent(const Node2D& node);

	template <DerivedComponent T>
	T getComponent(const Node2D& node);

	template <DerivedComponent T>
	T getComponent(uint64_t nodeId);

private:
	SystemManager() : _renderSystem{ RenderSystem(*this) }, _physicsSystem{ PhysicsSystem(*this) }
	{
	}

	RenderSystem _renderSystem;
	PhysicsSystem _physicsSystem;

	friend class Locator;
};

template <DerivedSystem T>
T& SystemManager::getSystem()
{
	if constexpr (std::is_same_v<T, PhysicsSystem>)
	{
		return _physicsSystem;
	}
	else if constexpr (std::is_same_v<T, RenderSystem>)
	{
		return _renderSystem;
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedSystem T>
void SystemManager::update(float delta)
{
	if constexpr (std::is_same_v<T, PhysicsSystem>)
	{
		_physicsSystem.update(delta);
	}
	else if constexpr (std::is_same_v<T, RenderSystem>)
	{
		_renderSystem.update(delta);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
void SystemManager::addComponent(const Node2D& node, T component)
{
	if constexpr (std::is_same_v<T, TransformComponent> || std::is_same_v<T, CollisionComponent>)
	{
		_physicsSystem.addComponent<T>(node, component);
	}
	else if constexpr (std::is_same_v<T, TriangulatedPrimitiveComponent>)
	{
		_renderSystem.addComponent<T>(node, component);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
void SystemManager::removeComponent(const Node2D& node)
{
	if constexpr (std::is_same_v<T, TransformComponent> || std::is_same_v<T, CollisionComponent>)
	{
		_physicsSystem.removeComponent<T>(node);
	}
	else if constexpr (std::is_same_v<T, TriangulatedPrimitiveComponent>)
	{
		_renderSystem.removeComponent<T>(node);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
T SystemManager::getComponent(const Node2D& node)
{
	return getComponent(node.getId());
}

template <DerivedComponent T>
T SystemManager::getComponent(uint64_t nodeId)
{
	if constexpr (std::is_same_v<T, TransformComponent>)
	{
		return _physicsSystem.getComponent<T>(nodeId);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}
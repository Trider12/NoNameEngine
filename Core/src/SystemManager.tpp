#pragma once

#include "Node2D.hpp"

template <DerivedSystem T>
T& SystemManager::getSystem()
{
	if constexpr (std::is_same_v<T, PhysicsSystem>)
	{
		return _physicsSystem;
	}
	else if constexpr (std::is_same_v<T, Graphics2DSystem>)
	{
		return _graphicsSystem;
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
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <ComponentType>
void SystemManager::addComponent(const Node2D& node)
{
	addComponent(node.getId());
}

template <ComponentType type>
void SystemManager::addComponent(uint64_t nodeId)
{
	if constexpr (type == ComponentType::Transform2D || type == ComponentType::Collision2D)
	{
		_physicsSystem.addComponent<type>(nodeId);
	}
	else
	{
		static_assert(always_false<type>, "No specialization!");
	}
}

template <DerivedComponent T>
T& SystemManager::getComponent(const Node2D& node)
{
	getComponent(node.getId());
}

template <DerivedComponent T>
T& SystemManager::getComponent(uint64_t nodeId)
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
#pragma once

#include "Node/Node2D.hpp"

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

template <ComponentType T>
void SystemManager::addComponent(const Node2D& node)
{
	if constexpr (T == ComponentType::Transform2D || T == ComponentType::Collision2D)
	{
		_physicsSystem.addComponent<T>(node);
	}
	else if constexpr (T == ComponentType::ColorRect || T == ComponentType::Texture)
	{
		_renderSystem.addComponent<T>(node);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <ComponentType T>
void SystemManager::removeComponent(const Node2D& node)
{
	if constexpr (T == ComponentType::Transform2D || T == ComponentType::Collision2D)
	{
		_physicsSystem.addComponent<T>(node);
	}
	else if constexpr (T == ComponentType::ColorRect || T == ComponentType::Texture)
	{
		_renderSystem.removeComponent<T>(node);
	}
	else
	{
		static_assert(always_false<T>, "No specialization!");
	}
}

template <DerivedComponent T>
T& SystemManager::getComponent(const Node2D& node)
{
	return getComponent<T>(node.getId());
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
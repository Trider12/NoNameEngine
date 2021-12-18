#pragma once

#include "ComponentArray.hpp"

namespace
{
	const uint8_t initialNumberOfComponents = 64;

	template <typename...> inline constexpr bool always_false = false;
}

class ComponentManager
{
public:
	template <DerivedComponent T>
	void update(float delta)
	{
		if constexpr (std::is_same_v<T, TransformComponent>)
		{
			for (auto& transformComponent : _transformComponentArray)
			{
				transformComponent.localTransform *= transformComponent.deferredTransform;
				transformComponent.globalTransform *= transformComponent.deferredTransform;
				transformComponent.deferredTransform = sf::Transform::Identity;
			}
		}
		else if constexpr (std::is_same_v<T, Graphics2DComponent>)
		{
			//TODO
		}
		else if constexpr (std::is_same_v<T, CollisionComponent>)
		{
			//TODO
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

	template <DerivedComponent T>
	void addComponent(std::shared_ptr<Node2D> node, T component)
	{
		addComponent(node->getId(), component);
	}

	template <DerivedComponent T>
	void addComponent(uint64_t nodeId, T component)
	{
		if constexpr (std::is_same_v<T, TransformComponent>)
		{
			_transformComponentArray.addComponent(nodeId, component);
		}
		else if constexpr (std::is_same_v<T, Graphics2DComponent>)
		{
			_graphics2DComponentArray.addComponent(nodeId, component);
		}
		else if constexpr (std::is_same_v<T, CollisionComponent>)
		{
			_collisionComponentArray.addComponent(nodeId, component);
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

	template <DerivedComponent T>
	T& getComponent(std::shared_ptr<Node2D> node)
	{
		getComponent(node->getId());
	}

	template <DerivedComponent T>
	T& getComponent(uint64_t nodeId)
	{
		if constexpr (std::is_same_v<T, TransformComponent>)
		{
			return _transformComponentArray.getComponent(nodeId);
		}
		else if constexpr (std::is_same_v<T, Graphics2DComponent>)
		{
			return _graphics2DComponentArray.getComponent(nodeId);
		}
		else if constexpr (std::is_same_v<T, CollisionComponent>)
		{
			return _collisionComponentArray.getComponent(nodeId);
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

private:
	ComponentManager() {}

	TransformComponentArray _transformComponentArray = TransformComponentArray(initialNumberOfComponents);
	Graphics2DComponentArray _graphics2DComponentArray = Graphics2DComponentArray(initialNumberOfComponents);
	CollisionComponentArray _collisionComponentArray = CollisionComponentArray(initialNumberOfComponents);

	friend class Locator;
};
#pragma once

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"

class PhysicsSystem : public System
{
public:
	void update(float delta);

	template <ComponentType T>
	void addComponent(const Node2D& node)
	{
		if constexpr (T == ComponentType::Transform2D)
		{
			_transformComponents.addComponent(node.getId());
		}
		else if constexpr (T == ComponentType::Collision2D)
		{
			_collisionComponents.addComponent(node.getId());
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

	template <DerivedComponent T>
	void addComponent(const Node2D& node, T component)
	{
		if constexpr (std::is_same_v<T, CollisionComponent>)
		{
			_collisionComponents.addComponent(node.getId(), component);
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

	template <ComponentType T>
	void removeComponent(const Node2D& node)
	{
		if constexpr (T == ComponentType::Transform2D)
		{
			_transformComponents.removeComponent(node.getId());
		}
		else
		{
			static_assert(always_false<T>, "No specialization!");
		}
	}

	template <DerivedComponent T>
	T& getComponent(const Node2D& node)
	{
		return getComponent<T>(node.getId());
	}

	template <DerivedComponent T>
	T& getComponent(uint64_t nodeId)
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

	void transformDeferred(const Node2D& node, sf::Transform transform);

private:
	PhysicsSystem(SystemManager& manager);

	ComponentArray<TransformComponent> _transformComponents;
	ComponentArray<CollisionComponent> _collisionComponents;

	friend class SystemManager;
};

static_assert (DerivedSystem<PhysicsSystem>);
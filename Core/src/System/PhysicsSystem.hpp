#pragma once

#include "System/System.hpp"
#include "Component/Component.hpp"
#include "Component/ComponentArray.hpp"
#include "DataStructures/QuadTree.hpp"

class PhysicsSystem : public System
{
public:
	void update(float delta);

	template <DerivedComponent T>
	void addComponent(const Node2D& node, T component)
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
	void removeComponent(const Node2D& node)
	{
		if constexpr (std::is_same_v<T, TransformComponent>)
		{
			_transformComponents.removeComponent(node.getId());
		}
		else if constexpr (std::is_same_v<T, CollisionComponent>)
		{
			_collisionComponents.removeComponent(node.getId());
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

	QuadTree<float> _quadTree;

	friend class SystemManager;
};

static_assert (DerivedSystem<PhysicsSystem>);
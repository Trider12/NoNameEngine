#pragma once

#include "System.hpp"
#include "Component.hpp"
#include "ComponentArray.hpp"

class PhysicsSystem : public System
{
public:
	void update(float delta);

	template <ComponentType type>
	void addComponent(uint64_t nodeId)
	{
		if constexpr (type == ComponentType::Transform2D)
		{
			_transformComponents.addComponent(nodeId);
		}
		else
		{
			static_assert(always_false<type>, "No specialization!");
		}
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

	friend class SystemManager;
};

static_assert (DerivedSystem<PhysicsSystem>);
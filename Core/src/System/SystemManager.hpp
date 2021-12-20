#pragma once

#include "System/Graphics2DSystem.hpp"
#include "System/PhysicsSystem.hpp"

class Node2D;

class SystemManager
{
public:
	void setRenderTarget(sf::RenderTarget& target)
	{
		_graphicsSystem.setRenderTarget(&target);
	}

	template <DerivedSystem T>
	T& getSystem();

	// shortcuts

	template <DerivedSystem T>
	void update(float delta);
	template <ComponentType>
	void addComponent(const Node2D& node);
	template <ComponentType type>
	void addComponent(uint64_t nodeId);
	template <DerivedComponent T>
	T& getComponent(const Node2D& node);
	template <DerivedComponent T>
	T& getComponent(uint64_t nodeId);

private:
	SystemManager() : _graphicsSystem{ Graphics2DSystem(*this) }, _physicsSystem{ PhysicsSystem(*this) }
	{
	}

	Graphics2DSystem _graphicsSystem;
	PhysicsSystem _physicsSystem;

	friend class Locator;
};

#include "SystemManager.tpp"
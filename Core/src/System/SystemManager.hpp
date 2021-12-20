#pragma once

#include "System/RenderSystem.hpp"
#include "System/PhysicsSystem.hpp"

class Node2D;

class SystemManager
{
public:
	void setRenderTarget(sf::RenderTarget& target)
	{
		_renderSystem.setRenderTarget(&target);
	}

	template <DerivedSystem T>
	T& getSystem();

	// shortcuts

	template <DerivedSystem T>
	void update(float delta);

	template <ComponentType T>
	void addComponent(const Node2D& node);

	template <ComponentType T>
	void removeComponent(const Node2D& node);

	template <DerivedComponent T>
	T& getComponent(const Node2D& node);
	template <DerivedComponent T>
	T& getComponent(uint64_t nodeId);

private:
	SystemManager() : _renderSystem{ RenderSystem(*this) }, _physicsSystem{ PhysicsSystem(*this) }
	{
	}

	RenderSystem _renderSystem;
	PhysicsSystem _physicsSystem;

	friend class Locator;
};

#include "SystemManager.tpp"
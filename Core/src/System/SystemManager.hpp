#pragma once

#include "System/RenderSystem.hpp"
#include "System/PhysicsSystem.hpp"

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
	void addComponent(const Node2D& node);
	template <DerivedComponent T>
	void addComponent(const Node2D& node, T component);

	template <DerivedComponent T>
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
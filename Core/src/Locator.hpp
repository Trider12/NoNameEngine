#pragma once

#include "Node2DManager.hpp"
#include "ComponentManager.hpp"

class Locator
{
public:
	static Locator& Instance() { return _instance; }

	ComponentManager& getComponentManager() { return _componentManager; }
	Node2DManager& getNode2DManager() { return _node2DManager; }

private:
	Locator() {}

	static Locator _instance;

	ComponentManager _componentManager = ComponentManager();
	Node2DManager _node2DManager = Node2DManager();
};
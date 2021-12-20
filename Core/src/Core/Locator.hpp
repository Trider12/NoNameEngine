#pragma once

#include "Node/Node2DManager.hpp"
#include "System/SystemManager.hpp"

class Locator
{
public:
	static Locator& getInstance() { return _instance; }

	SystemManager& getSystemManager() { return _systemManager; }
	Node2DManager& getNode2DManager() { return _node2DManager; }

private:
	Locator() {}

	static Locator _instance;

	SystemManager _systemManager = SystemManager();
	Node2DManager _node2DManager = Node2DManager();
};
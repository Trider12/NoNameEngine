#pragma once

#include <memory>

class Node2D;

class Node2DManager
{
public:
	std::shared_ptr<Node2D> getRoot();

	void setRoot(std::shared_ptr<Node2D> root);

private:
	Node2DManager();

	void registerNode(Node2D* node);

	uint64_t _nextId = 1;
	std::shared_ptr<Node2D> _root;

	friend class Node2D;
	friend class Locator;
};